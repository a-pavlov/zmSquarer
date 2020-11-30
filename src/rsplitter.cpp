#include "rsplitter.h"
#include  <QDebug>

RSplitter::RSplitter(const std::string& p, QObject *parent) :
    QObject(parent)
    , pattern(p.begin(), p.end())
    , currentRBuf(0)
    , outputRBuf(-1)
    , latestRBuf(-1) {
    rbuffers.append(QSharedPointer<RBuffer>(new RBuffer()));
    rbuffers.append(QSharedPointer<RBuffer>(new RBuffer()));
    rbuffers.append(QSharedPointer<RBuffer>(new RBuffer()));
}

RSplitter::~RSplitter() {}

void RSplitter::read(char* data, size_t size) {
    qDebug() << Q_FUNC_INFO << " size " << size;
    if (size <= 0) return;    
    QSharedPointer<RBuffer> prbuf = rbuffers[currentRBuf];
    prbuf->append(data, size);
    size_t remaining = prbuf->getUnmarkedSize();

    while(remaining > 0) {
        int ppos = prbuf->findCharacter(pattern[0]);
        qDebug() <<  "remaining " << remaining << " pattern starting position " << ppos;

        if (ppos != -1) {
            size_t patternFirstMatchPos = static_cast<size_t>(ppos);
            size_t bytesAvailableForMatching = prbuf->getUsed() - static_cast<size_t>(patternFirstMatchPos);
            bool notMatch = false;
            size_t ix = 1;

            qDebug() << "remain input bytes count " << bytesAvailableForMatching << " pattern size " << pattern.size();

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
                // swith buffers, set remaining to the unmarked amount and continue
                switchBufferMutex.lock();
                int accum = latestRBuf!=-1?latestRBuf:1;
                latestRBuf = currentRBuf;
                currentRBuf = accum;
                switchBufferMutex.unlock();
                prbuf = rbuffers[currentRBuf];
                prbuf->clear();
                // copy unmarked data from latest to the current to continue processing in the next buffer, skip pattern in the end
                prbuf->append(rbuffers[latestRBuf]->getUnmarkedPtr() + pattern.size(), rbuffers[latestRBuf]->getUnmarkedSize() - pattern.size());
                remaining = prbuf->getUnmarkedSize();
            } else if (ix == bytesAvailableForMatching) {
                Q_ASSERT(!notMatch);
                qDebug() << "end of buffer expanding to " << patternFirstMatchPos;
                // end of buffer before boundary matched - set content length to the beginning of the
                rbuffers[currentRBuf]->setContentLength(static_cast<size_t>(patternFirstMatchPos));
                qDebug() << "buffer unmarked size " << prbuf->getUnmarkedSize() << " used " << prbuf->getUsed();
                remaining = 0;
            } else {
                // not match
                Q_ASSERT(notMatch);
                remaining -= (patternFirstMatchPos + ix - prbuf->getContentLength());
                prbuf->setContentLength(static_cast<size_t>(patternFirstMatchPos + ix));
                qDebug() << "not match char pos " << patternFirstMatchPos << " body len " << ix;
            }
        } else {
            qDebug() << "pattern not found copy " << remaining;
            prbuf->expandContentLength(remaining);
            remaining = 0;
        }
    }
}
