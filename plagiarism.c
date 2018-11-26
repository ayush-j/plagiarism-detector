#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define d 256
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


int longestCommonSubsequece(char* str1, char* str2, int len1, int len2){
    int i, j;
    int LCS[len1+1][len2+1];

    for(i=0;i<=len1;i++)
        LCS[i][0]=0;
    for(j=0;j<=len2;j++)
        LCS[0][j]=0;

    for(i=1;i<=len1;i++){
        for(j=1;j<=len2;j++){
            if(str1[i-1]==str2[j-1]){
                LCS[i][j]=1+LCS[i-1][j-1];
            }
            else{
                LCS[i][j]=MAX(LCS[i-1][j],LCS[i][j-1]);
            }
        }
    }
 
    return LCS[len1][len2];
 
}

int KMP(const char* X, const char* Y, int m, int n){
    if (*Y == '\0' || n == 0) printf("Empty pattern");
    if (*X == '\0' || n > m) printf("No match");

    int next[n + 1];

    for (int i = 0; i < n + 1; i++)
        next[i] = 0;

    for (int i = 1; i < n; i++){
        int j = next[i + 1];

        while (j > 0 && Y[j] != Y[i])
            j = next[j];

        if (j > 0 || Y[j] == Y[i])
            next[i + 1] = j + 1;
    }

    int maxMatch = 0;
    for (int i = 0, j = 0; i < m; i++){
        if (*(X + i) == *(Y + j)){
            if (++j == n){
                //printf("found %d\n", i - j + 1);
                printf("\n%s", ""); // ???? doesn't work without this
            }
            maxMatch = MAX(maxMatch, j);
        }
        else if (j > 0){
            j = next[j];
            i--;
        }
    }
    return maxMatch;
}

int RK(char* txt, char* pat, int N, int M){
    int i, j;
    int p = 0;
    int t = 0;
    int h = 1;
    for (i = 0; i < M-1; i++)
        h = (h*d)%127;
    for (i = 0; i < M; i++){
        p = (d*p + pat[i])%127;
        t = (d*t + txt[i])%127;
    }

    int matches=0;
    for (i = 0; i <= N - M; i++){
        if (p == t){
            for (j = 0; j < M; j++)
                if (txt[i+j] != pat[j])
                    break;
            if (j == M){
                //printf("found %d \n", i);
                matches++;
            }
        }

        if ( i < N-M ){
            t = (d*(t - txt[i]*h) + txt[i+M])%127;

            if (t < 0)
                t = (t + 127);
        }
    }
    return matches;
}

void lower(char* str){
    int i = 0;
    for(i=0; str[i]; i++)
        str[i] = tolower(str[i]);
}

int main(){
    printf("\nPlagiarism Checker\n---------------------\n");
    printf("  %20s: %s\n", "Checking file", "target.txt");
    printf("  %20s: %s\n", "Source file", "source.txt");
    char wd1[100] = "C:\\Users\\ayush\\Dropbox\\MUJ V Sem\\DAA Lab\\DAA Project\\target.txt";
    char wd2[100] = "C:\\Users\\ayush\\Dropbox\\MUJ V Sem\\DAA Lab\\DAA Project\\source.txt";

    FILE *tfp = fopen(wd2, "r");
    if(tfp == NULL){
      perror("Error opening target file. ");
      exit(EXIT_FAILURE);
    }
    FILE *sfp = fopen(wd1, "r");
    if(sfp == NULL){
        perror("Error opening source file. ");
        exit(EXIT_FAILURE);
    }
    fseek(sfp, 0, SEEK_END);
    long sfsize = ftell(sfp);
    fseek(sfp, 0, SEEK_SET);
    char *string = malloc(sfsize + 1);
    fread(string, sfsize, 1, sfp);
    fclose(sfp);

    lower(string);

    double kmpRes = 0;
    double totalText = 0;
    char line[1000];
    while(fgets(line, 1000, tfp)){
        if(line[0]=='\n') continue;

        lower(line);

        // printf("\nProcessing: %d '%s'", strlen(line), line);
        kmpRes += KMP(string, line, sfsize, strlen(line));
        totalText += strlen(line);
    }

    printf("\n%32s: %.2f plagiarism", "Knuth-Morris-Pratt (KMP)", kmpRes/totalText*100);

    free(string);
    fclose(tfp);

    tfp = fopen(wd2, "r");
    if(tfp == NULL){
      perror("Error opening target file. ");
      exit(EXIT_FAILURE);
    }
    sfp = fopen(wd1, "r");
    if(sfp == NULL){
        perror("Error opening source file. ");
        exit(EXIT_FAILURE);
    }

    double lcssRes = 0, rkRes = 0;
    double lcss1, rk1;
    double totalLines = 0;
    totalText = 0;
    char pattern[1000], source[1000];
    int len_pattern, len_source;
    while(fgets(pattern, 1000, tfp)){
        if(pattern[0]=='\n') continue;

        lower(pattern);
        len_pattern = strlen(pattern);

        lcss1 = rk1 = 0;
        fseek(sfp, 0, SEEK_SET);
        while(fgets(source, 1000, sfp)){
            if(source[0]=='\n') continue;

            lower(source);
            len_source  = strlen(source);
            if(len_source>len_pattern){
                lcss1 = MAX(lcss1, longestCommonSubsequece(source, pattern, len_source, len_pattern));
                rk1 = MAX(rk1, RK(source, pattern, len_source, len_pattern));
            }
            else{
                lcss1 = MAX(lcss1, longestCommonSubsequece(pattern, source, len_pattern, len_source));
                rk1 = MAX(rk1, RK(pattern, source, len_pattern, len_source));
            }
        }
        lcssRes += lcss1;
        rkRes += rk1;
        totalText += len_pattern;
        totalLines += 1;
    }

    fclose(sfp);
    fclose(tfp);


    printf("\n%32s: %.2f plagiarism", "Longest Common Subseq. (LCSS)", lcssRes/totalText*100);
    printf("\n%32s: %.2f plagiarism", "Rabin Karp (RK)", rkRes/totalLines*100);

    return 0;
}

