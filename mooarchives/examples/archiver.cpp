
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include "mooarchiver.hpp"


enum archive_types { HV_ARCHIVE, DOMINATING_ARCHIVE, UNBOUND_ARCHIVE, UNDEFINED_ARCHIVE };
static const char * const archive_names[] =
  { "Hypervolume Archiver", "Dominating Archiver", "Unbounded Archiver", "UNDEFINED" };

static  long seed = 0;
static  unsigned max_size = 100;
static  unsigned dimension = 0;
static  int grid_levels = 0;
static  int seq_length = -1;
static  double epsilon = 0.0001;
static  const char * fileprefix = NULL;
static  const char * seq_filename = NULL;
static  enum archive_types archive_type = UNDEFINED_ARCHIVE;


bool 
strtovector_double (char * str, vector<double> &p)
{
  Solution s;
  char * endp = str;
  char * cursor = NULL;

  p.reserve (20);

  do {
    cursor = endp;
    double value = strtod (cursor, &endp);
    if (cursor == endp) break;
    p.push_back (value);
  } while (1);

  // not end of string: error
  while (*cursor != '\0') {
    if (!isspace(*cursor)) { return false; }
    cursor++;
  }
  
  if (p.size() == 0) return false;

  return true;
}

bool
readSolution (FILE *stream, Solution &s)
{
  char buffer[500];
  vector<double> v;
  static bool first_time = true;

  if (!fgets (buffer, 499, stream))
    return false;

  if (!strtovector_double(buffer, v))
    return false;

  if (first_time) {
    /* Set the number of objectives.  */
    Solution::Initialise (v.size());
    first_time = false;
  }

  s.setObjectives(v);
  return true;
}

void usage (void)
{
    printf("\n"
           "Usage: %s [OPTIONS]\n\n", "archiver");

    printf(
"Implementation of various multi-objective archiving algorithms.\n\n"
"Options:\n"
"Other optional parameters are:\n"
"-t integer : archive type\n");
  for (int i = 0; i < UNDEFINED_ARCHIVE; ++i)  {
    printf("\t%2d\t%s\n", i, archive_names[i]);
  }
  printf(
"-f character string : file name of sequence data\n"
"-N positive integer : capacity of the archive\n"
//"-k positive integer : number of objectives\n"
"-len positive integer : length of the input sequence\n"
"-s positive long : random seed\n"
"-o character string: output filename for sequence output, otherwise, print only the final result to stdout.\n"
"-g positive integer : number of levels of the adaptive grid; #grid regions=2^(l*k)\n"
"-e positive float : epsilon value for epsilon archivers\n"
"-v                : print version and copyright information\n"
"\n");
  printf ("Default values are:\n"
          "N = %d, len %d%s, seed = time(NULL), g = floor(log_2(N/2)), e = %f\n",
          max_size,
          seq_length, (seq_length < 0) ? " (all points)" : "",
          epsilon);
}

void
print_archive(BaseArchive<Solution> *archive, const char *fileprefix, int iteration)
{
  char name[1000];
  FILE *fp;
  
  assert (strlen(fileprefix) < 950);
  sprintf(name, "%s.%d", fileprefix, iteration);
  puts(name);
  if((fp = fopen(name, "w")))
    {
      archive->print(fp);
      fclose(fp);
    }
  else
    {
      perror(name);
      exit(1);
    }
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    usage();
    exit(1);
  }

  seed =  time(NULL);
  // printf("argc = %d\n", argc);
  for (int i = 1; i < argc; i += 2)    {
    if(strcmp("-h", argv[i]) == 0) {
	  usage();
	  exit(0);
    }
    else if(strcmp("-v", argv[i]) == 0) {
      exit(0);
    }
    else if(strcmp("-t", argv[i])==0)
      archive_type = archive_types(atoi(argv[i+1]));
    else if(strcmp("-N", argv[i])==0)
      max_size = atoi(argv[i+1]);
    else if(strcmp("-k", argv[i])==0)
      dimension = atoi(argv[i+1]);
    else if(strcmp("-g", argv[i])==0)
      grid_levels = atoi(argv[i+1]);
    else if(strcmp("-len", argv[i])==0)
      seq_length = atoi(argv[i+1]);
    else if(strcmp("-f", argv[i])==0)
      seq_filename = argv[i+1];
    else if(strcmp("-s", argv[i])==0)
      seed = atol(argv[i+1]);
    else if(strcmp("-e", argv[i])==0)
      epsilon = atof(argv[i+1]);
    else if(strcmp("-o", argv[i])==0)
      fileprefix = argv[i+1];
    else
      {
        fprintf(stderr, "Undefined command line parameter entered. "
                "Do \"./archiver -h\" for help with parameters.\n");
        exit(1);
      }
  }

  if (seq_filename == NULL) {
    fprintf(stderr, "error: not input file (use -f file)\n");
    exit (1);
  }

  FILE *fich = fopen(seq_filename, "r");
  if (!fich) {
    perror (seq_filename);
    exit(1);
  }

  Solution s;
  
  if (!readSolution(fich, s)) {
    fprintf(stderr, "error reading input file %s\n", seq_filename);
    exit (1);
  }

  if (dimension == 0) {
    dimension = s.num_objs();
  }

  Random rng (seed);
  BaseArchive<Solution> * archive;

  switch (archive_type) {
  case UNBOUND_ARCHIVE:
    archive = new UnboundedArchive<Solution> (s.num_objs());
    break;
  case DOMINATING_ARCHIVE:
    archive = new DominatingArchive<Solution> (max_size, s.num_objs());
    break;
  case HV_ARCHIVE:
    archive = new HVArchive<Solution> (max_size, s.num_objs(), rng);
    break;
  default:
    printf ("error: undefined archive type: %d\n", archive_type);
    exit(1);
  }

  int iteration = 0;
  do {
    iteration++;
    archive->add(s);
    if (fileprefix)
      print_archive(archive, fileprefix, iteration);
  } while (readSolution (fich, s) && iteration != seq_length);
  
  if (!fileprefix)
    archive->print();

  delete archive;

  return 0;
}

