# zmSquarer
Squarer for ZoneMinder. 

### Limitations:
Supports only http protocol for now and no-authentication.

### The project goal
* Showing multiple cameras in one window like ZoneMinder montage page.
* Minimal CPU consuming. Decompressing MJPEG using libjpeg-turbo or Qt image.
* Convenient view capabilities. Build montage window. Link low res sources to hi res sources using color groups.

### Current state
MVP application for ZM on http and no authorization.

### Source code used:
* [Motion](https://github.com/Motion-Project/motion): an example of MJPEG parsing. Not in the build now.
* [LibJpeg-Turbo](https://github.com/libjpeg-turbo/libjpeg-turbo): decompress JPEG frames


![zmSquarer](images/zmSquarer.png)
![zmSquarer2](images/zmSquarer2.png)
