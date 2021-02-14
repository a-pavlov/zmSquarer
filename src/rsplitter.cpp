#include "rsplitter.h"
#include <algorithm>
#include  <QDebug>

RSplitter::RSplitter(QObject *parent) :
    QObject(parent)
    , currentRBuf(0)
    , outputRBuf(2)
    , latestRBuf(-1)
    , frameCounter(0)
    , lastBufferSize(-1) {
    rbuffers.append(QSharedPointer<RBuffer>(new RBuffer()));
    rbuffers.append(QSharedPointer<RBuffer>(new RBuffer()));
    rbuffers.append(QSharedPointer<RBuffer>(new RBuffer()));
}

RSplitter::~RSplitter() {
    if (!timeouts.isEmpty()) {
        qDebug() << "average timeout msec: " << std::accumulate(timeouts.begin(), timeouts.end(), 0)/timeouts.size();
        qDebug() << "max timeout msec:     " <<  *std::max_element(timeouts.begin(), timeouts.end());
        qDebug() << "min timeout msec:     " <<  *std::min_element(timeouts.begin(), timeouts.end());
    } else {
        qDebug() << "no statistics are available";
    }
}

void RSplitter::read(char* data, size_t size) {
    //qDebug() << Q_FUNC_INFO << " size " << size << " current rbuf " << currentRBuf;
    if (size <= 0) return;
    QSharedPointer<RBuffer> prbuf = rbuffers[currentRBuf];
    prbuf->append(data, size);
    processCB(prbuf);
}

void RSplitter::processCB(QSharedPointer<RBuffer> prbuf) {
    size_t remaining = prbuf->getUnmarkedSize();

    while(remaining > 0) {
        int ppos = (lastBufferSize != -1 && (prbuf->getUsed() < (lastBufferSize*9/10)))?-1:prbuf->findCharacter(pattern[0]);

        //qDebug() <<  "remaining " << remaining << " pattern starting position " << ppos;

        if (ppos != -1) {
            size_t patternFirstMatchPos = static_cast<size_t>(ppos);
            size_t bytesAvailableForMatching = prbuf->getUsed() - static_cast<size_t>(patternFirstMatchPos);
            bool notMatch = false;
            size_t ix = 1;

            //qDebug() << "remain input bytes count " << bytesAvailableForMatching << " pattern size " << pattern.size();

            for(; ix < qMin(pattern.size(), bytesAvailableForMatching); ++ix) {
                if (pattern[ix] != prbuf->getChar(patternFirstMatchPos + ix)) {
                    notMatch = true;
                    break;
                }
            }

            if (ix == pattern.size()) {
                // end of pattern, pattern fully matched - copy data to output buffer
                Q_ASSERT(!notMatch);
                // expand content length with position of the boundary
                prbuf->setContentLength(static_cast<size_t>(patternFirstMatchPos));
                prbuf->setFrameIndex(++frameCounter);
                prbuf->calculateSoiPosition();
                lastBufferSize = prbuf->getContentLength();
                QTime currentTime = QTime::currentTime();

                //qDebug() << "frame " << frameCounter << " content length " << prbuf->getContentLength() << " timeout msec: " << frameTime.msecsTo(currentTime);

                if (timeouts.size() < 1000) {
                    timeouts.append(frameTime.msecsTo(currentTime));
                }

                frameTime = currentTime;
                // swith buffers, set remaining to the unmarked amount and continue
                switchBufferMutex.lock();
                int accum = latestRBuf!=-1?latestRBuf:1;
                latestRBuf = currentRBuf;
                currentRBuf = accum;
                //qDebug() << "latest " << latestRBuf << " current " << currentRBuf;
                switchBufferMutex.unlock();
                prbuf = rbuffers[currentRBuf];
                prbuf->clear();
                // copy unmarked data from latest to the current to continue processing in the next buffer, skip pattern in the end
                prbuf->append(rbuffers[latestRBuf]->getUnmarkedPtr() + pattern.size(), rbuffers[latestRBuf]->getUnmarkedSize() - pattern.size());
                remaining = prbuf->getUnmarkedSize();
            } else if (ix == bytesAvailableForMatching) {
                Q_ASSERT(!notMatch);
                //qDebug() << "end of buffer expanding to " << patternFirstMatchPos;
                // end of buffer before boundary matched - set content length to the beginning of the
                rbuffers[currentRBuf]->setContentLength(static_cast<size_t>(patternFirstMatchPos));
                //qDebug() << "buffer unmarked size " << prbuf->getUnmarkedSize() << " used " << prbuf->getUsed();
                remaining = 0;
            } else {
                // not match
                Q_ASSERT(notMatch);
                remaining -= (patternFirstMatchPos + ix - prbuf->getContentLength());
                prbuf->setContentLength(static_cast<size_t>(patternFirstMatchPos + ix));
                //qDebug() << "not match char pos " << patternFirstMatchPos << " body len " << ix;
            }
        } else {
            //qDebug() << "pattern not found copy " << remaining;
            prbuf->expandContentLength(remaining);
            remaining = 0;
        }
    }
}

void RSplitter::reset() {
    QMutexLocker ml(&switchBufferMutex);
    currentRBuf = 0;
    outputRBuf  = 2;
    latestRBuf  = -1;
    frameCounter= 0;
    lastBufferSize = -1;
    frameTime = QTime();
    outputTime = QTime();
    foreach (QSharedPointer<RBuffer> buffer, rbuffers) {
        buffer->reset();
    }
}

void RSplitter::setPattern(const QString& p) {
    std::string str = p.toStdString();
    std::vector<char> newPattern(str.begin(), str.end());
    pattern.swap(newPattern);
}

QSharedPointer<RBuffer> RSplitter::getOutputBuffer() {
    QMutexLocker ml(&switchBufferMutex);

    // return null if no ready buffer or last output time greater than frame time - no new frame arrived
    if (latestRBuf == -1 || (!outputTime.isNull() && outputTime > frameTime)) {
        return QSharedPointer<RBuffer>();
    }

    outputTime = QTime::currentTime();
    int buf = latestRBuf;
    latestRBuf = outputRBuf;
    outputRBuf = buf;
    //qDebug() << "output rbuff " << outputRBuf;
    Q_ASSERT(outputRBuf != -1);
    return rbuffers[outputRBuf];
}
