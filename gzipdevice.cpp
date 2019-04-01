#include "gzipdevice.h"

#include <QFile>

#include <stdexcept>

GzipDevice::GzipDevice(QFile &file, QObject *parent) :
    QIODevice(parent),
    m_file(file)
{
    if (!m_file.isOpen())
        throw std::runtime_error("file is not open");

    setOpenMode(QIODevice::ReadOnly);

    // Prepare inflater status
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    // Initialize inflater
    m_result = inflateInit2(&strm, 15 + 16);
    if (m_result != Z_OK)
        throw std::runtime_error("could not init z_stream");
}

GzipDevice::~GzipDevice()
{
    inflateEnd(&strm);
}

bool GzipDevice::isSequential() const
{
    return true;
}

bool GzipDevice::atEnd() const
{
    return m_result == Z_STREAM_END;
}

qint64 GzipDevice::readData(char *data, qint64 maxlen)
{
    if (strm.avail_in == 0)
    {
        strm.next_in = reinterpret_cast<unsigned char *>(m_readBuffer);
        strm.avail_in = m_file.read(m_readBuffer, m_readBufferSize);
    }

    strm.next_out = reinterpret_cast<unsigned char*>(data);
    strm.avail_out = maxlen;

    m_result = inflate(&strm, Z_NO_FLUSH);

    switch (m_result) {
    case Z_NEED_DICT:
        throw std::runtime_error("decompression failed: Z_NEED_DICT");
    case Z_DATA_ERROR:
        throw std::runtime_error("decompression failed: Z_DATA_ERROR");
    case Z_MEM_ERROR:
        throw std::runtime_error("decompression failed: Z_MEM_ERROR");
    case Z_STREAM_ERROR:
        throw std::runtime_error("decompression failed: Z_STREAM_ERROR");
    }

    return maxlen-strm.avail_out;
}

qint64 GzipDevice::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
    qFatal("no writes allowed in GzipDevice!");
    return -1;
}
