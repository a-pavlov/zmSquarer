#ifndef RSPLITTER_H
#define RSPLITTER_H

#include <string>
#include <cassert>

#include <QObject>
#include <QList>
#include <QMutex>
#include <QSharedPointer>
#include <QTime>

#include "rbuffer.h"

class RSplitter : public QObject
{
    Q_OBJECT
public:
    typedef std::vector<char>::size_type size_type;
    explicit RSplitter(const std::string& pattern, QObject *parent = nullptr);    
    virtual ~RSplitter();
    virtual void read(char* data, size_t size);
    void setPattern(const QString&);
    const char* getPattern() const {
        return &pattern[0];
    }

    QSharedPointer<RBuffer> getCurrentRBuffer() {
        assert(currentRBuf != -1);
        return rbuffers[currentRBuf];
    }

    QSharedPointer<RBuffer> getLatestRBuffer() {
        if (latestRBuf != -1) {
            assert(latestRBuf < rbuffers.size());
            return rbuffers[latestRBuf];
        }

        return QSharedPointer<RBuffer>();
    }

    QSharedPointer<RBuffer> getOutputBuffer();

    void processCB(QSharedPointer<RBuffer>);
private:
    std::vector<char> pattern;
    int currentRBuf;
    int outputRBuf;
    int latestRBuf;
    QList<QSharedPointer<RBuffer>> rbuffers;
    QMutex switchBufferMutex;
    long frameCounter;
    QTime frameTime;
    QTime outputTime;
    QList<int> timeouts;
    int lastBufferSize;
signals:

public slots:
};

#endif // RSPLITTER_H
