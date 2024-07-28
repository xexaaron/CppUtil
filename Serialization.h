#include <filesystem>
#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <sstream>

namespace Utility { 
    /** 
    * @brief BinaryStream used for treating an object as binary data, can only hold one object at a time.
    */
    class BinaryStream {
    public:
        BinaryStream() {}

        template <typename T>
        BinaryStream(const T& structure) {
            *this << structure;
        }

        template <typename T>
        BinaryStream& operator<<(const T& structure) {
            const unsigned char* data = reinterpret_cast<const unsigned char*>(&structure);
            ss.str(""); 
            ss.write(reinterpret_cast<const char*>(data), sizeof(T));
            return *this;
        }

        template <typename T>
        BinaryStream& operator>>(T& structure) {
            unsigned char* data = reinterpret_cast<unsigned char*>(&structure);
            ss.read(reinterpret_cast<char*>(data), sizeof(T));
            return *this;
        }

        std::string str() const {
            return ss.str();
        }

        std::vector<std::string> Readable() const {
            std::string binary_data = ss.str();
            std::vector<std::string> rows;
            size_t num_bytes = binary_data.size();
            size_t cols = static_cast<size_t>(std::sqrt(num_bytes)) + 1;
            if (cols < 1) cols = 1;
            size_t num_rows = (num_bytes + cols - 1) / cols;
            for (size_t row = 0; row < num_rows; ++row) {
                std::stringstream ss_row;
                for (size_t col = 0; col < cols; ++col) {
                    size_t index = row * cols + col;
                    if (index < num_bytes) {
                        std::bitset<8> bits(binary_data[index]);
                        ss_row << bits << " ";
                    }
                }
                rows.push_back(ss_row.str());
            }

            return rows;
        }

    private:
        std::stringstream ss;
    };

    template <>
    BinaryStream& BinaryStream::operator<<<std::string>(const std::string& binary_string) {
        ss.str("");
        ss << binary_string;
        return *this;
    }

    class Serialization {
    public:
        template <typename T>
        static void Serialize(const std::filesystem::path& path, const T& object) {
            auto fpath = path;
            
            if (!fpath.has_extension()) {
                fpath.replace_extension(".dat");
            }
            
            BinaryStream bs;
            bs << object;
            
            std::ofstream ofs(fpath, std::ios::binary);
            if (ofs.is_open()) {
                ofs << bs.str();
                ofs.close();
            } else {
                std::cerr << "Failed to open file : " << fpath << "." << std::endl;
                std::cerr << "Reason              : " << std::strerror(errno) << "." << std::endl;
            }
        }

        template <typename T>
        static void Deserialize(const std::filesystem::path& path, T& object) {
            auto fpath = path;
            if (!fpath.has_extension()) {
                fpath.replace_extension(".dat");
            }

            std::ifstream ifs(fpath, std::ios::binary);
            if (ifs.is_open()) {
                std::stringstream buffer;
                buffer << ifs.rdbuf();
                BinaryStream bs;
                bs << buffer.str();
                bs >> object;
                ifs.close();
            }
            else {
                std::cerr << "Failed to open file : " << fpath << "." << std::endl;
                std::cerr << "Reason              : " << std::strerror(errno) << "." << std::endl;
            }
        }
    };
}