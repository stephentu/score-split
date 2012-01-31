#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

static int base10_digts_needed(int nitems) {
  int i;
  i = 1;
  nitems /= 10;
  while (nitems > 0) {
    nitems /= 10;
    i++;
  }
  return i;
}

static int score_split(const char* filename, const float* scores, int nscores) {
  int i, j, alloced, nlines, curlines, plines, nfmtchars, needed, eof;
  char c, xferbuf[8192], buf[256], buffmt[128], *p;
  FILE *fp, *fpcur;
  float sum;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("Could not open file");
    return 1;
  }

  nlines = 0;
  while ((c = fgetc(fp)) != EOF) {
    if (c == '\n') nlines++;
  }
  fseek(fp, 0, SEEK_SET);

  sum = 0.0;
  for (i = 0; i < nscores; i++) sum += scores[i];

  nfmtchars = base10_digts_needed(nscores);
  sprintf(buffmt, "%%s_%%0%dd", nfmtchars);

  plines = 0; eof = 0;
  for (i = 0; i < nscores; i++) {
    curlines = eof ? 0 :
      ((i == nscores - 1) ?
         nlines - plines : round((scores[i] / sum) * ((float) nlines)));
    alloced = 0;
    if ((needed = (strlen(filename) + 1 + nfmtchars + 1)) > sizeof(buf)) {
      p = (char *) malloc(needed);
      if (p == NULL) {
        fprintf(stderr, "Could not allocate memory\n");
        return 1;
      }
      alloced = 1;
    } else {
      p = buf;
    }

    sprintf(p, buffmt, filename, i);
    fpcur = fopen(p, "w");

    j = 0;
    while (j < curlines) {
      if (fgets(xferbuf, sizeof(xferbuf), fp) != NULL) {
        fputs(xferbuf, fpcur);
        j++; plines++;
      } else if (feof(fp)) {
        eof = 1;
        break;
      } else {
        // error condition
        fprintf(stderr, "Error in writing to file\n");
        return 1;
      }
    }
    fflush(fpcur);
    fclose(fpcur);

    if (alloced) {
      free(p);
    }
  }

  fclose(fp);
  return 0;
}

static int test2(const char* test, const char* a, const char* b) {
  return strcmp(test, a) == 0 || strcmp(test, b) == 0;
}

static const char *usage =
  "[USAGE] score-split "
    "(-f|--filename) filename "
    "(-s|--scores) score0 score1 ... scoreN";

int main(int argc, char** argv) {
  int i, j, k;
  const char* filename;
  float f;
  float* scores;

  filename = NULL;
  scores = NULL;
  for (i = 1; i < argc;) {
    if (test2(argv[i], "-s", "--scores")) {
      if ((i + 1 == argc) ||
          test2(argv[i + 1], "-f", "--filename")) {
        fprintf(stderr, "No scores given to %s\n", argv[i]);
        fprintf(stderr, "%s\n", usage);
        return 1;
      }
      i++;
      scores = (float *) malloc(sizeof(float) * (argc - i));
      if (scores == NULL) {
        fprintf(stderr, "Could not allocate memory\n");
        fprintf(stderr, "%s\n", usage);
        return 1;
      }
      j = 0;
      while (i < argc) {
        if (test2(argv[i], "-f", "--filename")) {
          break;
        }
        f = strtof(argv[i], NULL);
        if (errno == ERANGE) {
          fprintf(stderr, "Could not read score (%s)", argv[i]);
          fprintf(stderr, "%s\n", usage);
          return 1;
        }
        if (f <= 0.0) {
          fprintf(stderr, "Negative or 0 score not allowed\n");
          fprintf(stderr, "%s\n", usage);
          return 1;
        }
        scores[j++] = f;
        i++;
      }
    } else if (test2(argv[i], "-f", "--filename")) {
      if ((i + 1 == argc) ||
          test2(argv[i + 1], "-s", "--scores")) {
        fprintf(stderr, "No file given to %s\n", argv[i]);
        fprintf(stderr, "%s\n", usage);
        return 1;
      }
      filename = argv[i + 1];
      i = i + 2;
    }
  }

  if (filename == NULL) {
    fprintf(stderr, "No filename given\n");
    fprintf(stderr, "%s\n", usage);
    return 1;
  }

  if (scores == NULL) {
    fprintf(stderr, "No scores given\n");
    fprintf(stderr, "%s\n", usage);
    return 1;
  }

  if ((k = score_split(filename, scores, j))) {
    return k;
  }

  return 0;
}
