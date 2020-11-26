/*   This file is part of Motion.
 *
 *   Motion is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   Motion is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Motion.  If not, see <https://www.gnu.org/licenses/>.
 */

/*    motion.c
 *
 *    Detect changes in a video stream.
 *    Copyright 2000 by Jeroen Vreeken (pe1rxq@amsat.org)
 *
 */
#include "translate.h"
#include "motion.h"
#include "util.h"
#include "logger.h"


#define IMAGE_BUFFER_FLUSH ((unsigned int)-1)

/**
 * tls_key_threadnr
 *
 *   TLS key for storing thread number in thread-local storage.
 */
pthread_key_t tls_key_threadnr;

/**
 * global_lock
 *
 *   Protects any global variables (like 'threads_running') during updates,
 *   to prevent problems with multiple threads updating at the same time.
 */
//pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t global_lock;

/**
 * cnt_list
 *
 *   List of context structures, one for each main Motion thread.
 */
struct context **cnt_list = NULL;

/**
 * threads_running
 *
 *   Keeps track of number of Motion threads currently running. Also used
 *   by 'main' to know when all threads have exited.
 */
volatile int threads_running = 0;

/* Set this when we want main to end or restart */
volatile unsigned int finish = 0;

/* Log file used instead of stderr and syslog */
FILE *ptr_logfile = NULL;

/**
 * restart
 *
 *   Differentiates between a quit and a restart. When all threads have
 *   finished running, 'main' checks if 'restart' is true and if so starts
 *   up again (instead of just quitting).
 */
unsigned int restart = 0;


/**
 * image_ring_resize
 *
 * This routine is called from motion_loop to resize the image precapture ringbuffer
 * NOTE: This function clears all images in the old ring buffer

 * Parameters:
 *
 *      cnt      Pointer to the motion context structure
 *      new_size The new size of the ring buffer
 *
 * Returns:     nothing
 */
static void image_ring_resize(struct context *cnt, int new_size)
{
    /*
     * Only resize if :
     * Not in an event and
     * decreasing at last position in new buffer
     * increasing at last position in old buffer
     * e.g. at end of smallest buffer
     */
    if (cnt->event_nr != cnt->prev_event) {
        int smallest;

        if (new_size < cnt->imgs.image_ring_size) {  /* Decreasing */
            smallest = new_size;
        } else {  /* Increasing */
            smallest = cnt->imgs.image_ring_size;
        }

        if (cnt->imgs.image_ring_in == smallest - 1 || smallest == 0) {
            MOTION_LOG(NTC, TYPE_ALL, NO_ERRNO
                ,_("Resizing pre_capture buffer to %d items"), new_size);

            /* Create memory for new ring buffer */
            struct image_data *tmp;
            tmp = mymalloc(new_size * sizeof(struct image_data));

            /*
             * Copy all information from old to new
             * Smallest is 0 at initial init
             */
            if (smallest > 0) {
                memcpy(tmp, cnt->imgs.image_ring, sizeof(struct image_data) * smallest);
            }

            /* In the new buffers, allocate image memory */
            {
                int i;
                for(i = smallest; i < new_size; i++) {
                    tmp[i].image_norm = mymalloc(cnt->imgs.size_norm);
                    memset(tmp[i].image_norm, 0x80, cnt->imgs.size_norm);  /* initialize to grey */
                    if (cnt->imgs.size_high > 0) {
                        tmp[i].image_high = mymalloc(cnt->imgs.size_high);
                        memset(tmp[i].image_high, 0x80, cnt->imgs.size_high);
                    }
                }
            }

            /* Free the old ring */
            free(cnt->imgs.image_ring);

            /* Point to the new ring */
            cnt->imgs.image_ring = tmp;
            cnt->current_image = NULL;

            cnt->imgs.image_ring_size = new_size;

            cnt->imgs.image_ring_in = 0;
            cnt->imgs.image_ring_out = 0;
        }
    }
}

/**
 * image_ring_destroy
 *
 * This routine is called when we want to free the ring
 *
 * Parameters:
 *
 *      cnt      Pointer to the motion context structure
 *
 * Returns:     nothing
 */
static void image_ring_destroy(struct context *cnt)
{
    int i;

    /* Exit if don't have any ring */
    if (cnt->imgs.image_ring == NULL) {
        return;
    }

    /* Free all image buffers */
    for (i = 0; i < cnt->imgs.image_ring_size; i++) {
        free(cnt->imgs.image_ring[i].image_norm);
        if (cnt->imgs.size_high >0 ) {
            free(cnt->imgs.image_ring[i].image_high);
        }
    }

    /* Free the ring */
    free(cnt->imgs.image_ring);

    cnt->imgs.image_ring = NULL;
    cnt->current_image = NULL;
    cnt->imgs.image_ring_size = 0;
}

/**
 * context_init
 *
 *   Initializes a context struct with the default values for all the
 *   variables.
 *
 * Parameters:
 *
 *   cnt - the context struct to destroy
 *
 * Returns: nothing
 */
void context_init(struct context *cnt)
{
   /*
    * We first clear the entire structure to zero, then fill in any
    * values which have non-zero default values.  Note that this
    * assumes that a NULL address pointer has a value of binary 0
    * (this is also assumed at other places within the code, i.e.
    * there are instances of "if (ptr)").  Just for possible future
    * changes to this assumption, any pointers which are intended
    * to be initialised to NULL are listed within a comment.
    */

    memset(cnt, 0, sizeof(struct context));
    cnt->noise = 255;
    cnt->lastrate = 25;

    //memcpy(&cnt->track, &track_template, sizeof(struct trackoptions));

    cnt->pipe = -1;
    cnt->mpipe = -1;

    cnt->vdev = NULL;    /*Init to NULL to check loading parms vs web updates*/
    cnt->netcam = NULL;
    cnt->rtsp = NULL;
    cnt->rtsp_high = NULL;

}

/**
 * context_destroy
 *
 *   Destroys a context struct by freeing allocated memory, calling the
 *   appropriate cleanup functions and finally freeing the struct itself.
 *
 * Parameters:
 *
 *   cnt - the context struct to destroy
 *
 * Returns: nothing
 */
void context_destroy(struct context *cnt)
{
    unsigned int j;

    /* Free memory allocated for config parameters */
    /*for (j = 0; config_params[j].param_name != NULL; j++) {
        if (config_params[j].copy == copy_string ||
            config_params[j].copy == copy_uri ||
            config_params[j].copy == read_camera_dir) {
            void **val;
            val = (void *)((char *)cnt+(int)config_params[j].conf_value);
            if (*val) {
                free(*val);
                *val = NULL;
            }
        }
    }*/

    free(cnt);
}

/**
 * sig_handler
 *
 *  Our SIGNAL-Handler. We need this to handle alarms and external signals.
 */
static void sig_handler(int signo)
{
    int i;

    /*The FALLTHROUGH is a special comment required by compiler.  Do not edit it*/
    switch(signo) {
    case SIGALRM:
        /*
         * Somebody (maybe we ourself) wants us to make a snapshot
         * This feature triggers snapshots on ALL threads that have
         * snapshot_interval different from 0.
         */
        if (cnt_list) {
            i = -1;
            while (cnt_list[++i]) {
                if (cnt_list[i]->conf.snapshot_interval) {
                    cnt_list[i]->snapshot = 1;
                }

            }
        }
        break;
    case SIGUSR1:
        /* Trigger the end of a event */
        if (cnt_list) {
            i = -1;
            while (cnt_list[++i]) {
                cnt_list[i]->event_stop = TRUE;
            }
        }
        break;
    case SIGHUP:
        restart = 1;
        /*
         * Fall through, as the value of 'restart' is the only difference
         * between SIGHUP and the ones below.
         */
         /*FALLTHROUGH*/
    case SIGINT:
        /*FALLTHROUGH*/
    case SIGQUIT:
        /*FALLTHROUGH*/
    case SIGTERM:
        /*
         * Somebody wants us to quit! We should finish the actual
         * movie and end up!
         */

        if (cnt_list) {
            i = -1;
            while (cnt_list[++i]) {
                cnt_list[i]->webcontrol_finish = TRUE;
                cnt_list[i]->event_stop = TRUE;
                cnt_list[i]->finish = 1;
                /*
                 * Don't restart thread when it ends,
                 * all threads restarts if global restart is set
                 */
                 cnt_list[i]->restart = 0;
            }
        }
        /*
         * Set flag we want to quit main check threads loop
         * if restart is set (above) we start up again
         */
        finish = 1;
        break;
    case SIGSEGV:
        exit(0);
    case SIGVTALRM:
        printf("SIGVTALRM went off\n");
        break;
    }
}

/**
 * sigchild_handler
 *
 *   This function is a POSIX compliant replacement of the commonly used
 *   signal(SIGCHLD, SIG_IGN).
 */
static void sigchild_handler(int signo)
{
    (void)signo;

    #ifdef WNOHANG
        while (waitpid(-1, NULL, WNOHANG) > 0) {
            continue;
        }
    #endif /* WNOHANG */
    return;
}

/**
 * setup_signals
 *   Attaches handlers to a number of signals that Motion need to catch.
 */
static void setup_signals(void)
{
    /*
     * Setup signals and do some initialization. 1 in the call to
     * 'motion_startup' means that Motion will become a daemon if so has been
     * requested, and argc and argc are necessary for reading the command
     * line options.
     */
    struct sigaction sig_handler_action;
    struct sigaction sigchild_action;

    #ifdef SA_NOCLDWAIT
        sigchild_action.sa_flags = SA_NOCLDWAIT;
    #else
        sigchild_action.sa_flags = 0;
    #endif
    sigchild_action.sa_handler = sigchild_handler;
    sigemptyset(&sigchild_action.sa_mask);
    #ifdef SA_RESTART
        sig_handler_action.sa_flags = SA_RESTART;
    #else
        sig_handler_action.sa_flags = 0;
    #endif
    sig_handler_action.sa_handler = sig_handler;
    sigemptyset(&sig_handler_action.sa_mask);

    /* Enable automatic zombie reaping */
    sigaction(SIGCHLD, &sigchild_action, NULL);
    sigaction(SIGPIPE, &sigchild_action, NULL);
    sigaction(SIGALRM, &sig_handler_action, NULL);
    sigaction(SIGHUP, &sig_handler_action, NULL);
    sigaction(SIGINT, &sig_handler_action, NULL);
    sigaction(SIGQUIT, &sig_handler_action, NULL);
    sigaction(SIGTERM, &sig_handler_action, NULL);
    sigaction(SIGUSR1, &sig_handler_action, NULL);

    /* use SIGVTALRM as a way to break out of the ioctl, don't restart */
    sig_handler_action.sa_flags = 0;
    sigaction(SIGVTALRM, &sig_handler_action, NULL);
}

/**
 * motion_remove_pid
 *   This function remove the process id file ( pid file ) before motion exit.
 */
void motion_remove_pid(void)
{
    if ((cnt_list[0]->daemon) && (cnt_list[0]->conf.pid_file) && (restart == 0)) {
        if (!unlink(cnt_list[0]->conf.pid_file)) {
            MOTION_LOG(NTC, TYPE_ALL, NO_ERRNO, _("Removed process id file (pid file)."));
        } else {
            MOTION_LOG(ERR, TYPE_ALL, SHOW_ERRNO, _("Error removing pid file"));
        }
    }

    if (ptr_logfile) {
        MOTION_LOG(NTC, TYPE_ALL, NO_ERRNO, _("Closing logfile (%s)."),
                   cnt_list[0]->conf.log_file);
        myfclose(ptr_logfile);
        set_log_mode(LOGMODE_NONE);
        ptr_logfile = NULL;
    }

}


static int init_camera_type(struct context *cnt)
{

    cnt->camera_type = CAMERA_TYPE_UNKNOWN;

    #ifdef HAVE_MMAL
        if (cnt->conf.mmalcam_name) {
            cnt->camera_type = CAMERA_TYPE_MMAL;
            return 0;
        }
    #endif // HAVE_MMAL

    if (cnt->conf.netcam_url) {
        if ((strncmp(cnt->conf.netcam_url,"mjpeg",5) == 0) ||
            (strncmp(cnt->conf.netcam_url,"v4l2" ,4) == 0) ||
            (strncmp(cnt->conf.netcam_url,"file" ,4) == 0) ||
            (strncmp(cnt->conf.netcam_url,"rtmp" ,4) == 0) ||
            (strncmp(cnt->conf.netcam_url,"rtsp" ,4) == 0)) {
            cnt->camera_type = CAMERA_TYPE_RTSP;
        } else {
            cnt->camera_type = CAMERA_TYPE_NETCAM;
        }
        return 0;
    }

    #ifdef HAVE_BKTR
        if (strncmp(cnt->conf.video_device,"/dev/bktr",9) == 0) {
            cnt->camera_type = CAMERA_TYPE_BKTR;
            return 0;
        }
    #endif // HAVE_BKTR

    #ifdef HAVE_V4L2
        if (cnt->conf.video_device) {
            cnt->camera_type = CAMERA_TYPE_V4L2;
            return 0;
        }
    #endif // HAVE_V4L2


    MOTION_LOG(ERR, TYPE_ALL, NO_ERRNO
        , _("Unable to determine camera type (MMAL, Netcam, V4L2, BKTR)"));
    return -1;

}

static void init_text_scale(struct context *cnt)
{

    /* Consider that web interface may change conf values at any moment.
     * The below can put two sections in the image so make sure that after
     * scaling does not occupy more than 1/4 of image (10 pixels * 2 lines)
     */

    cnt->text_scale = cnt->conf.text_scale;
    if (cnt->text_scale <= 0) {
        cnt->text_scale = 1;
    }

    if ((cnt->text_scale * 10 * 2) > (cnt->imgs.width / 4)) {
        cnt->text_scale = (cnt->imgs.width / (4 * 10 * 2));
        if (cnt->text_scale <= 0) {
            cnt->text_scale = 1;
        }
        MOTION_LOG(WRN, TYPE_ALL, NO_ERRNO
            ,_("Invalid text scale.  Adjusted to %d"), cnt->text_scale);
    }

    if ((cnt->text_scale * 10 * 2) > (cnt->imgs.height / 4)) {
        cnt->text_scale = (cnt->imgs.height / (4 * 10 * 2));
        if (cnt->text_scale <= 0) {
            cnt->text_scale = 1;
        }
        MOTION_LOG(WRN, TYPE_ALL, NO_ERRNO
            ,_("Invalid text scale.  Adjusted to %d"), cnt->text_scale);
    }

    /* If we had to modify the scale, change conf so we don't get another message */
    cnt->conf.text_scale = cnt->text_scale;

}


int motion_init(struct context *cnt)
{
    FILE *picture;
    int indx, retcd;

    util_threadname_set("ml",cnt->threadnr,cnt->conf.camera_name);

    /* Store thread number in TLS. */
    pthread_setspecific(tls_key_threadnr, (void *)((unsigned long)cnt->threadnr));

    cnt->currenttime_tm = mymalloc(sizeof(struct tm));
    cnt->eventtime_tm = mymalloc(sizeof(struct tm));
    /* Init frame time */
    cnt->currenttime = time(NULL);
    localtime_r(&cnt->currenttime, cnt->currenttime_tm);

    cnt->smartmask_speed = 0;

    /*
     * We initialize cnt->event_nr to 1 and cnt->prev_event to 0 (not really needed) so
     * that certain code below does not run until motion has been detected the first time
     */
    cnt->event_nr = 1;
    cnt->prev_event = 0;
    cnt->lightswitch_framecounter = 0;
    cnt->detecting_motion = 0;
    cnt->event_user = FALSE;
    cnt->event_stop = FALSE;

    /* Make sure to default the high res to zero */
    cnt->imgs.width_high = 0;
    cnt->imgs.height_high = 0;
    cnt->imgs.size_high = 0;
    cnt->movie_passthrough = cnt->conf.movie_passthrough;

    MOTION_LOG(NTC, TYPE_ALL, NO_ERRNO
        ,_("Camera %d started: motion detection %s"),
        cnt->camera_id, cnt->pause ? _("Disabled"):_("Enabled"));

    if (!cnt->conf.target_dir) {
        cnt->conf.target_dir = mystrdup(".");
    }

    if (init_camera_type(cnt) != 0 ) {
        return -3;
    }

    if ((cnt->camera_type != CAMERA_TYPE_RTSP) && (cnt->movie_passthrough)) {
        MOTION_LOG(WRN, TYPE_ALL, NO_ERRNO,_("Pass-through processing disabled."));
        cnt->movie_passthrough = FALSE;
    }

    if ((cnt->conf.height == 0) || (cnt->conf.width == 0)) {
        MOTION_LOG(WRN, TYPE_ALL, NO_ERRNO
            ,_("Invalid configuration dimensions %dx%d"),cnt->conf.height,cnt->conf.width);
        cnt->conf.height = DEF_HEIGHT;
        cnt->conf.width = DEF_WIDTH;
        MOTION_LOG(WRN, TYPE_ALL, NO_ERRNO
            ,_("Using default dimensions %dx%d"),cnt->conf.height,cnt->conf.width);
    }
    if (cnt->conf.width % 8) {
        MOTION_LOG(CRT, TYPE_NETCAM, NO_ERRNO
            ,_("Image width (%d) requested is not modulo 8."), cnt->conf.width);
        cnt->conf.width = cnt->conf.width - (cnt->conf.width % 8) + 8;
        MOTION_LOG(CRT, TYPE_NETCAM, NO_ERRNO
            ,_("Adjusting width to next higher multiple of 8 (%d)."), cnt->conf.width);
    }
    if (cnt->conf.height % 8) {
        MOTION_LOG(CRT, TYPE_NETCAM, NO_ERRNO
            ,_("Image height (%d) requested is not modulo 8."), cnt->conf.height);
        cnt->conf.height = cnt->conf.height - (cnt->conf.height % 8) + 8;
        MOTION_LOG(CRT, TYPE_NETCAM, NO_ERRNO
            ,_("Adjusting height to next higher multiple of 8 (%d)."), cnt->conf.height);
    }
    if (cnt->conf.width  < 64) {
        cnt->conf.width  = 64;
    }
    if (cnt->conf.height < 64) {
        cnt->conf.height = 64;
    }

    return 0;
}
