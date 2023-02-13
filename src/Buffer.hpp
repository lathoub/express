constexpr size_t rawBufferSize = 1024;

class Buffer {
public:
  byte buffer[rawBufferSize]{};
  size_t byteOffset = 0;
  size_t length = 0;

  static Buffer *from(const String &data,
                      const String &encoding = F("base64")) {
    Buffer *buffer = new Buffer();

    buffer->length = data.length();
    memcpy(buffer->buffer, data.c_str(), data.length());

    LOG_V(F("encoded length"), buffer->length);

    return buffer;
  }

  String toString() {
    return "AAABAAEAEBAQAAAAAAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAA"
           "AAAAAEAAAAAAAAAAAAAAA/"
           "4QAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
           "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEREQAAAAAAEAAAEAAAA"
           "AEAAAABAAAAEAAAAAAQAAAQAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAEAABAAAAAAAA"
           "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD//wAA//8AAP//"
           "AAD8HwAA++8AAPf3AADv+wAA7/sAAP//AAD//wAA+98AAP//AAD//wAA//8AAP//"
           "AAD//wAA";
  }
};
