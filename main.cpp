#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <string>

using namespace std;

const int buffer_size = 1024;

enum FILE_TYPE {
    C,
    CPP
};

string self;

string input_file;
string output_file;
string language;

string upper_filename;

FILE_TYPE file_type;

static const string description = "\t[option]...\n"
        "\t-i\tinput file\n"
        "\t-o\toutput file\n"
        "\t-l\tlanguage c|cpp\n"
        "\t-h\thelp\n";

void usage() {
    cout << self << description << endl;
}

void add_header_patten(FILE *file) {
    fprintf(file, "#ifndef %s_H\n", upper_filename.c_str());
    fprintf(file, "#define %s_H\n\n", upper_filename.c_str());
}

void add_content_patten(FILE *input, FILE *output, FILE_TYPE file_type) {
    if(file_type == C) {
        fprintf(output, "const unsigned char %s_data[] = ", output_file.c_str());
    } else {
        fprintf(output, "namespace %s {\n", upper_filename.c_str());
        fprintf(output, "\tconst unsigned char data[] = ");
    }

    fprintf(output, "{ ");

    unsigned char buffer[buffer_size];
    fseek(input, 0, SEEK_SET);
    size_t nread = fread(buffer, 1, buffer_size, input);
    while(nread > 0) {
        for(size_t i = 0; i < nread; ++ i) {
            fprintf(output, "0x%02x,", buffer[i]);
        }
        nread = fread(buffer, 1, buffer_size, input);
    }

    long input_file_size = ftell(input);
    if(input_file_size > 0) {
        fseek(output, -1, SEEK_CUR);
    }
    fprintf(output, " };\n");

    if(file_type == C) {
        fprintf(output, "const long %s_size = %ld;\n\n", output_file.c_str(), input_file_size);
    } else {
        fprintf(output, "\tconst long size = %ld;\n", input_file_size);
        fprintf(output, "}\n\n");
    }
}

void add_footer_patten(FILE *output) {
    fprintf(output, "#endif // end of %s_H", upper_filename.c_str());
}

void copy(FILE *input, FILE *output) {
    fseek(input, 0, SEEK_SET);
    fseek(output, 0, SEEK_SET);
    char buffer[buffer_size];
    size_t nread = fread(buffer, 1, buffer_size, input);
    while(nread > 0) {
        fwrite(buffer, 1, nread, output);
        nread = fread(buffer, 1, buffer_size, input);
    }
}

int main(int argc, char *argv[])
{
    self = argv[0];
    int opt;
    while((opt = getopt(argc, argv, "i:o:l:h")) != -1) {
        switch(opt) {
        case 'i':
            cout << "input : " << optarg << endl;
            input_file = optarg;
            break;
        case 'o':
            cout << "output : " << optarg << endl;
            output_file = optarg;
            break;
        case 'l':
            cout << "language : " << optarg << endl;
            language = optarg;
            break;
        case 'h':
            usage();
            return 0;
        case ':':
            cerr << "option needs a value" << endl;
            break;
        case '?':
            cerr << "unknow option : " << char(optopt) << endl;
            break;
        }
    }

    if(input_file.empty()) {
        cerr << "no input file" << endl;
        usage();
        return -1;
    }

    if(output_file.empty()) {
        cerr << "no output file" << endl;
        usage();
        return -1;
    }

    if(language.empty()) {
        cerr << "language must be c or cpp" << endl;
        usage();
        return -1;
    }

    if(language == "c") {
        file_type = C;
    } else if(language == "cpp") {
        file_type = CPP;
    } else {
        cerr << "language must be c or cpp" << endl;
        return -1;
    }

    FILE *input = fopen(input_file.c_str(), "r+");
    if(!input) {
        cerr << input_file << " open failed." << endl;
        return -1;
    }

    FILE *tmp = tmpfile();
    if(!tmp) {
        cerr << "open or create temp failed." << endl;
        fclose(tmp);
        return -1;
    }

    upper_filename = output_file;
    for(int i = 0; i < output_file.length(); ++ i) {
        upper_filename[i] = toupper(output_file[i]);
    }

    add_header_patten(tmp);
    add_content_patten(input, tmp, file_type);
    add_footer_patten(tmp);

    fclose(input);

    FILE *output = fopen((output_file + ".h").c_str(), "w+b");
    if(!output) {
        cerr << "failed to open " << output_file << endl;
        fclose(tmp);
        return -1;
    }

    copy(tmp, output);
    fclose(tmp);
    fclose(output);

    return 0;
}

