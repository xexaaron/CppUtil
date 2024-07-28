#include <filesystem>
#include <fstream>
#include <sstream>

namespace Utility {

class File {
public:
    File(const std::filesystem::path& path);
    ~File() = default;
    
    bool IsValid() const;
    const std::filesystem::path& Path() const;
    bool Read(char* buffer, size_t* out_buff_size, std::ios_base::openmode mode = std::ios::in) const;
    bool Read(std::string* buffer, std::ios_base::openmode mode = std::ios::in) const;
    bool Write(const std::string& data, std::ios::openmode mode = std::ios::app) const;

private:
    std::filesystem::path m_Path;
};

}

namespace Utility {
    File::File(const std::filesystem::path& path) : m_Path(path) {}

    bool File::Read(char* buffer, size_t* out_buffer_size, std::ios_base::openmode mode) const {
        if (out_buffer_size) {
            std::ifstream ifs(m_Path, mode);
            if (ifs.is_open()) {
                size_t bytes = 0;
                ifs.seekg(0, std::ios::end);
                std::streampos file_size = ifs.tellg();
                ifs.tellg();
                buffer = new char[static_cast<size_t>(file_size) + 1];
                ifs.read(buffer, file_size);
                buffer[file_size] = '\0';
                *out_buffer_size = static_cast<size_t>(file_size);
                ifs.close();
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    bool File::Read(std::string* buffer, std::ios_base::openmode mode) const {
        if (buffer) {
            std::ifstream ifs(m_Path, mode);
            if (ifs.is_open()) {
                std::stringstream ss;
                ss << ifs.rdbuf();
                *buffer = ss.str();
                ifs.close();
                return true;
            }
            return false;
        } else {
            return false;
        }
    }

    bool File::Write(const std::string& data, std::ios::openmode mode) const {
        std::ofstream ofs(m_Path, mode);
        if (ofs.is_open()) {
            ofs << data;
            ofs.close();
            return true;
        }
        return false;
    }

    const std::filesystem::path& File::Path() const {
        return m_Path;
    }

}