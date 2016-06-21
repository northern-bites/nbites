#include <iostream>

int bzip_deflate(std::istream &source, std::ostream &dest, int level = 9);
int bzip_inflate(std::istream &source, std::ostream &dest);
