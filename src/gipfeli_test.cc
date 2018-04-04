
#if _MSC_VER
#include <filesystem>
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "gipfeli/gipfeli.h"
#include "gipfeli/stubs-internal.h"

using std::cout;
using std::endl;
using std::vector;

#if _MSC_VER
const std::filesystem::path testdata_prefix("..\\testdata");
#else
const char testdata_prefix[] = "testdata/";
#endif

const int kRepetitions = 101;
const int kFiles = 9;

static struct {
  const char* label;
  const char* filename;
} files[] = {
      {"html", "html"},
      {"urls", "urls.10K"},
      {"jpg", "fireworks.jpeg"},
      {"pdf", "paper-100k.pdf"},
      {"html4", "html_x_4"},
      {"txt1", "alice29.txt"},
      {"txt2", "asyoulik.txt"},
      {"txt3", "lcet10.txt"},
      {"txt4", "plrabn12.txt"},
};

void ReadTestDataFile(const string& filename, string* content) {
  std::ifstream ifs(filename.c_str());
  content->assign((std::istreambuf_iterator<char>(ifs)),
                  (std::istreambuf_iterator<char>()));
}

double start;
double stop;
double Timestamp() {
#if _MSC_VER
  struct _timespec64 t;
  _timespec64_get(&t, TIME_UTC);
  return t.tv_nsec / 1000 + t.tv_sec * 1000000LL;
#else
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_usec + t.tv_sec * 1000000LL;
#endif
}
void ResetTimer() { start = Timestamp(); }
double GetElapsedTime() {
  stop = Timestamp();
  return (stop - start) / 1000000.0;
}

// Return true if compression and uncompression was successful
bool TestFile(const string& label, const string& filename,
              size_t* original_size, size_t* compressed_size,
              double* compression_time, double* uncompression_time) {
  // Read the input
  string original;
#if _MSC_VER
  ReadTestDataFile((SOLUTION_DIR / testdata_prefix / filename).string(),
                   &original);
#else
  ReadTestDataFile(testdata_prefix + filename, &original);
#endif
  *original_size = original.size();

  // Init compressor and compress
  string compressed;
  google::compression::Compressor* compressor =
      google::compression::NewGipfeliCompressor();
  ResetTimer();
  *compressed_size = compressor->Compress(original, &compressed);
  *compression_time = GetElapsedTime();

  // Decompress and destroy compressor
  string uncompressed;
  ResetTimer();
  bool success = compressor->Uncompress(compressed, &uncompressed);
  *uncompression_time = GetElapsedTime();
  delete compressor;

  return success && (original == uncompressed);
}

int main() {
  // Collect performance data
  vector<size_t> original_size(kFiles, 0);
  vector<size_t> compressed_size(kFiles, 0);
  vector<double> sum_of_compression_time(kFiles, 0);
  vector<double> sum_of_uncompression_time(kFiles, 0);
  vector<bool> valid(kFiles, true);

  for (int i = 0; i < kRepetitions; i++)
    for (int j = 0; j < kFiles; j++) {
      double compression_time = 0;
      double uncompression_time = 0;
      bool success =
          TestFile(files[j].label, files[j].filename, &original_size[j],
                   &compressed_size[j], &compression_time, &uncompression_time);
      if (!success) valid[j] = false;
      sum_of_compression_time[j] += compression_time;
      sum_of_uncompression_time[j] += uncompression_time;
    }

  // Output test and benchmark results
  for (int i = 0; i < kFiles; i++) {
    cout << files[i].label << " [" << files[i].filename
         << "] Compression ratio: "
         << (1000 * compressed_size[i] / original_size[i]) / 10.0
         << "%  Verification test: " << (valid[i] ? "PASSED." : "FAILED.")
         << endl;
    cout << "Compression speed: "
         << (original_size[i] / (sum_of_compression_time[i] / kRepetitions)) /
                1000000 << " M/s."
         << " Uncompression speed: "
         << (original_size[i] / (sum_of_uncompression_time[i] / kRepetitions)) /
                1000000 << " M/s." << endl;
  }
}
