#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

class pgm_t
{
public:
    pgm_t(const std::string &filename)
    {
        FILE *fp;
        fp = fopen(filename.c_str(), "rb");
        assert(fp != NULL);

        char temp[3];
        fscanf(fp, "%s %d %d %d", temp, &this->_width, &this->_height, &this->_max_gray);
        assert(strcmp(temp, "P5") == 0);
        fgetc(fp);
        this->_ptr = reinterpret_cast<uint8_t *>(malloc(this->_height * this->_width));
        fread(this->_ptr, this->_height * this->_width, 1, fp);

        fclose(fp);
    };

    pgm_t(uint32_t width, uint32_t height)
    {
        this->_height = height;
        this->_width = width;
        this->_max_gray = 255;
        this->_ptr = reinterpret_cast<uint8_t *>(malloc(this->_height * this->_width));
        memset(this->_ptr, 0, this->_width * this->_height);
    };

    ~pgm_t() { free(this->_ptr); }

    pgm_t(pgm_t &other)
    {
        this->_width = other.width();
        this->_height = other.height();
        this->_max_gray = other.max_gray();
        this->_ptr = reinterpret_cast<uint8_t *>(malloc(this->_height * this->_width));
        memcpy(this->_ptr, other.ptr(), this->_height * this->_width);
    }
    pgm_t &operator=(pgm_t &other)
    {
        this->_width = other.width();
        this->_height = other.height();
        this->_max_gray = other.max_gray();
        this->_ptr = reinterpret_cast<uint8_t *>(malloc(this->_height * this->_width));
        memcpy(this->_ptr, other.ptr(), this->_height * this->_width);
        return *this;
    }

    void write(const std::string &filename)
    {
        FILE *fp;
        fp = fopen(filename.c_str(), "wb");
        assert(fp != NULL);

        fprintf(fp, "P5\n%d %d\n%d\n", this->_width, this->_height, this->_max_gray);
        fwrite(this->_ptr, this->_height * this->_width, 1, fp);

        fclose(fp);
    }

    uint32_t height() { return this->_height; }
    uint32_t width() { return this->_width; }
    uint32_t max_gray() { return this->_max_gray; }
    uint8_t *ptr() { return this->_ptr; }

private:
    uint32_t _height;
    uint32_t _width;
    uint32_t _max_gray;
    uint8_t *_ptr;
};