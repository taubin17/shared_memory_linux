#include "cpp_essentials/configuration/tomlplusplus/toml.hpp"

#include "ByteString.hpp"

#include <array>
#include <iostream>
#include <random>
#include <cstdint>
#include <fstream>
#include <filesystem>

using DataType = char;
using DataContainer = std::vector<DataType>;

// Function to generate random data.
void randomize_data(DataContainer& data)
{
  std::random_device random_device;
  std::mt19937 rng(random_device());

  std::uniform_int_distribution<std::mt19937::result_type> random_value(
    std::numeric_limits<DataType>::min(), std::numeric_limits<DataType>::max());

  for (auto & element : data)
    element = random_value(rng);

  return;
}

// Function to write data to memory
void write_to_file(const std::string filepath, const DataContainer&& data)
{
  const std::string_view serialized_data(data.data(), data.size());

  std::ofstream output_file(filepath, std::ios::binary);

  output_file << serialized_data;

  // std::cout << "Writing. Filename: " << filepath << " --- " 
  //   << "Last Byte: " << static_cast<int> (serialized_data.back()) << std::endl;

  output_file.flush();

  return;
}

// Function to read data from memory.
DataContainer read_from_file(const std::filesystem::path filepath, const size_t size)
{
  std::ifstream input_file(filepath, std::ios::binary);

  DataContainer data;
  data.resize(size);
  input_file.read(data.data(), size);

  // std::cout << "Reading. Filename: " << filepath << " --- " 
  //   << "Last Byte: " << static_cast<int> (data.back()) << std::endl;

  return data;
}

int main(int argc, char * argv[])
{
  if (argc != 3)
  {
    std::cerr << "Invalid argument count." << std::endl;
    return -1;
  }

  const std::filesystem::path configuration_filepath(argv[1]);
  const std::filesystem::path tailored_configuration_filepath(argv[2]);

  if (!std::filesystem::exists(configuration_filepath))
  {
    std::cerr << "Configuration file provided does not exist." << std::endl;
    return -3;
  }

  if (!std::filesystem::exists(tailored_configuration_filepath))
  {
    std::cerr << "Tailored configuration file provided does not exist." << std::endl;
    return -4;
  }

  std::cout << "Parsing tailored configuration file: " 
    << std::filesystem::absolute(tailored_configuration_filepath) << std::endl;
  auto tailored_configuration = toml::parse_file(tailored_configuration_filepath.string());


  const auto directoryNode = tailored_configuration["ParentDirectory"].value_exact<std::string>();
  if (!directoryNode)
  {
    std::cerr << "No output directory specified in the tailored configuration file." << std::endl;
    return -2;
  }

  const std::filesystem::directory_entry directory {*directoryNode};

  if (!directory.exists())
    std::filesystem::create_directories(directory);

  std::cout << "Parsing common configuration file: " 
    << std::filesystem::absolute(configuration_filepath) << std::endl;
  auto common_configuration = toml::parse_file(configuration_filepath.string());

  const size_t height = common_configuration["Height"].value_or(640ul);
  const size_t width = common_configuration["Width"].value_or(512ul);
  const size_t bytes_per_pixel = common_configuration["BytesPerPixel"].value_or(2ul);
  const size_t num_images = common_configuration["ImageCount"].value_or(32ul);

  const size_t data_size = height * width * bytes_per_pixel;

  std::cout << "Memory to be used: " << ByteString(data_size * num_images).str() << std::endl;

  // For each file, generate random data, and write it to the file.
  for (size_t i = 0; i < num_images; ++i)
  {
    std::filesystem::path filepath = directory;
    filepath /= ("shared_memory_test_" + std::to_string(i));
    DataContainer data;
    data.resize(data_size);
    randomize_data(data);
    write_to_file(filepath, std::move(data));
  }

  // For each file, read the data.
  for (size_t i = 0; i < num_images; ++i)
  {
    std::filesystem::path filepath = directory;
    filepath /= ("shared_memory_test_" + std::to_string(i));
    const auto data = read_from_file(filepath, data_size);
  }

  return 0;
}