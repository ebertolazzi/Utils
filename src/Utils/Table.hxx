/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2021                                                      |
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

///
/// eof: Table.hxx
///

/*\

  Based on terminal-table:

  https://github.com/Bornageek/terminal-table

  Copyright 2015 Andreas Wilhelm

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

\*/

namespace Utils {

  namespace Table {

    using std::string;
    using std::vector;

    class Table;

    typedef int integer;

    enum Alignment { LEFT, RIGHT, CENTER };

    class Style {
    private:

      char m_border_top          = '-';
      char m_border_top_mid      = '+';
      char m_border_top_left     = '+';
      char m_border_top_right    = '+';

      char m_border_bottom       = '-';
      char m_border_bottom_mid   = '+';
      char m_border_bottom_left  = '+';
      char m_border_bottom_right = '+';

      char m_border_left         = '|';
      char m_border_left_mid     = '+';

      char m_border_mid          = '-';
      char m_border_mid_mid      = '+';

      char m_border_right        = '|';
      char m_border_right_mid    = '+';

      char m_border_middle       = '|';

      integer m_padding_left     = 1;
      integer m_padding_right    = 1;

      Alignment m_Align = Alignment::LEFT;

      integer m_Width = 0;

    public:

      Style() {}

      char border_top() const { return m_border_top; }
      void border_top( char borderStyle ) { m_border_top = borderStyle; }

      char border_top_mid() const { return m_border_top_mid; }
      void border_top_mid( char borderStyle ) { m_border_top_mid = borderStyle; }

      char border_top_left() const { return m_border_top_left; }
      void border_top_left( char borderStyle ) { m_border_top_left = borderStyle; }

      char border_top_right() const { return m_border_top_right; }
      void border_top_right( char borderStyle ) { m_border_top_right = borderStyle; }

      char border_bottom() const { return m_border_bottom; }
      void border_bottom( char borderStyle ) { m_border_bottom = borderStyle; }

      char border_bottom_mid() const { return m_border_bottom_mid; }
      void border_bottom_mid( char borderStyle ) { m_border_bottom_mid = borderStyle; }

      char border_bottom_left() const { return m_border_bottom_left; }
      void border_bottom_left( char borderStyle ) { m_border_bottom_left = borderStyle; }

      char border_bottom_right() const { return m_border_bottom_right; }
      void border_bottom_right( char borderStyle) { m_border_bottom_right = borderStyle; }

      char border_left() const { return m_border_left; }
      void border_left( char borderStyle ) { m_border_left = borderStyle; }

      char border_left_mid() const { return m_border_left_mid; }
      void border_left_mid( char borderStyle ) { m_border_left_mid = borderStyle; }

      char border_mid() const { return m_border_mid; }
      void border_mid( char borderStyle ) { m_border_mid = borderStyle; }

      char border_mid_mid() const { return m_border_mid_mid; }
      void border_mid_mid( char borderStyle ) { m_border_mid_mid = borderStyle; }

      char border_right() const { return m_border_right; }
      void border_right( char borderStyle ) { m_border_right = borderStyle; }

      char border_right_mid() const { return m_border_right_mid; }
      void border_right_mid( char borderStyle ) { m_border_right_mid = borderStyle; }

      char border_middle() const { return m_border_middle; }
      void border_middle( char borderStyle ) { m_border_middle = borderStyle; }

      integer padding_left() const { return m_padding_left; }
      void    padding_left( integer padding ) { m_padding_left = padding; }

      integer padding_right() const { return m_padding_right; }
      void    padding_right( integer padding ) { m_padding_right = padding; }

      Alignment alignment() const { return m_Align; }
      void      alignment( Alignment align ) { m_Align = align; }

      integer width() const { return m_Width; }
      void    width( integer width ) { m_Width = width; }
    };

    class Cell {
    private:
      Table *   m_Table    = nullptr;
      string    m_Value    = "";
      Alignment m_Align    = Alignment::LEFT;
      integer   m_col_span = 1;
      integer   m_Width    = 0;

    public:

      Cell() {}

      Cell(
        Table*         table,
        string const & val      = "",
        integer        col_span = 1
      );

      string const & value() const { return m_Value; }
      void value( string const & val ) { m_Value = val; }

      Alignment alignment() const { return m_Align; }
      void alignment( Alignment align ) { m_Align = align; }

      integer col_span() const { return m_col_span; }
      void col_span( integer col_span ) { m_col_span = col_span; }

      integer width( integer col ) const;
      integer height() const;

      integer maximum_line_width() const;

      string line( integer idx ) const;
      void trim_line( string & line ) const;

      string render( integer line, integer col ) const;
    };

    class Row {
    protected:
      typedef vector<Cell>             vecCell;
      typedef std::vector<std::string> vecstr;

      Table * m_Table = nullptr;
      vecCell m_Cells;

    public:

      Row() {}

      Row(
        Table *        table,
        vecstr const & cells = vecstr()
      );

      Table const * table() const { return m_Table; }

      //vecCell & cells() { return m_Cells; }
      void cells( vecstr const & cells );

      integer num_cells() const { return integer(m_Cells.size()); }
      integer cell_width( integer idx ) const;
      void cell_col_span( integer idx, integer span );

      void cell( string const & value );
      //Cell& cell( integer idx ) { return m_Cells[idx]; }

      Cell const & operator [] ( integer idx ) const { return m_Cells[idx]; }
      Cell       & operator [] ( integer idx )       { return m_Cells[idx]; }

      integer height() const;

      string render() const;
    };

    class Table {
    public:
      typedef vector<Row>    vecRow;
      typedef vector<Cell>   vecCell;
      typedef vector<string> vecstr;
      typedef vector<vecstr> vecvecstr;
      typedef int integer;

    private:
      Style  m_Style;
      string m_Title;
      Row    m_Headings;
      vecRow m_Rows;

    public:

      Table() {}

      Table(
        Style     const & style,
        vecvecstr const & rows = vecvecstr()
      )
      : m_Style(style) {
        this->rows(rows);
      }

      void
      setup(
        Style     const & style,
        vecvecstr const & rows = vecvecstr()
      ) {
        m_Style = style;
        this->rows(rows);
      }

      void align_column( integer n, Alignment align );
      void add_row( vecstr const & row );

      integer cell_spacing() const;
      integer cell_padding() const;

      vecCell column( integer n ) const;
      integer column_width( integer n ) const;
      integer num_columns() const;

      Style const & style() const { return m_Style; }

      void style( Style const & style ) { m_Style = style; }

      string const & title() const { return m_Title; }

      void title( string const & title ) { m_Title = title; }

      Row const & headings() const { return m_Headings; }

      void headings( vecstr const & headings );

      Row       & row( integer n );
      Row const & row( integer n ) const;

      Row       & operator [] ( integer n )       { return this->row(n); }
      Row const & operator [] ( integer n ) const { return this->row(n); }

      Cell       & operator () ( integer i, integer j )       { return (*this)[i][j]; }
      Cell const & operator () ( integer i, integer j ) const { return (*this)[i][j]; }

      vecRow const & rows() const { return m_Rows; }
      void rows( vecvecstr const & rows );

      string
      render_separator(
        char const left,
        char const mid,
        char const right,
        char const sep
      ) const;

      string render() const;
    };
  }
}

inline
Utils::ostream_type&
operator << ( Utils::ostream_type& stream, Utils::Table::Row const & row ) {
  return stream << row.render();
}

inline
Utils::ostream_type&
operator << ( Utils::ostream_type& stream, Utils::Table::Table const & table ) {
  return stream << table.render();
}

///
/// eof: Table.hxx
///
