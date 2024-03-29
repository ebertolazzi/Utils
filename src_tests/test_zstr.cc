/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2017                                                      |
 |                                                                          |
 |         , __                 , __                                        |
 |        /|/  \               /|/  \                                       |
 |         | __/ _   ,_         | __/ _   ,_                                |
 |         |   \|/  /  |  |   | |   \|/  /  |  |   |                        |
 |         |(__/|__/   |_/ \_/|/|(__/|__/   |_/ \_/|/                       |
 |                           /|                   /|                        |
 |                           \|                   \|                        |
 |                                                                          |
 |      Enrico Bertolazzi                                                   |
 |      Dipartimento di Ingegneria Industriale                              |
 |      Universita` degli Studi di Trento                                   |
 |      email: enrico.bertolazzi@unitn.it                                   |
 |                                                                          |
\*--------------------------------------------------------------------------*/

#include "Utils.hh"
#include "Utils_zstr.hh"

#include <fstream>
#include <string>

using std::cout;

int
main() {
  try {
    // WRITE
    {
      std::ofstream file("test.txt.gz",std::ios::binary);
      zstr::ostream gzfile(file);
      for ( int i = 0; i < 100; ++i ) {
        gzfile << i << " ";
        gzfile << "pippo ";
        gzfile << "pluto ";
        gzfile << "paperino ";
        gzfile << "paperone ";
        gzfile << "nonna papera\n";
      }
      gzfile.flush(); // must be done before close!
      file.close();
    }
    // READ
    {
      cout << "read compressed file----------------\n";
      std::ifstream file("test.txt.gz",std::ios::binary);
      zstr::istream gzfile(file);
      while( gzfile.good() ) {
        std::string line;
        std::getline( gzfile, line );
        Utils::to_upper(line);
        cout << line << '\n';
      }
      file.close();
      cout << "done--------------------------------\n";
    }
  } catch ( std::exception const & exc ) {
    cout << "Error: " << exc.what() << '\n';
  } catch ( ... ) {
    cout << "Unknown error\n";
  }
  cout << "All done folks\n\n";
  return 0;
}
