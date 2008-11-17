
// This file is part of TOOL, a robotics interaction and development
// package created by the Northern Bites RoboCup team of Bowdoin College
// in Brunswick, Maine.
//
// TOOL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TOOL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TOOL.  If not, see <http://www.gnu.org/licenses/>.

package edu.bowdoin.robocup.TOOL.SQL;


public abstract class SQLRequest {

    protected static String SELECT = "SELECT";
    protected static String FROM = "FROM";
    protected static String AS = "AS";
    protected static String WHERE = "WHERE";
    protected static String ORDER = "ORDER BY";


    private StringBuffer buf;
    private boolean pretty;

    public SQLRequest() {
        regenerate();
    }

    protected SQLRequest(boolean toGen) {
        if (toGen)
            regenerate();
    }

    public String toString() {
        return buf.toString();
    }

    public String prettyString() {
        pretty = true;

        StringBuffer tmp = buf;
        regenerate();
        String s = buf.toString();
        buf = tmp;

        return s;
    }

    public boolean isPretty() {
        return pretty;
    }

    public StringBuffer getBuffer() {
        return buf;
    }

    public void regenerate() {
        buf = new StringBuffer();

        generateSelect(buf);   buf.append(' ');
        if (pretty)            buf.append('\n');
        generateFrom(buf);     buf.append(' ');
        if (pretty)            buf.append('\n');
        generateWhere(buf);    buf.append(' ');
        if (pretty)            buf.append("\n  ");
        generateGroup(buf);    buf.append(' ');
        if (pretty)            buf.append("\n  ");
        generateOrder(buf);    buf.append(' ');
    }

    protected abstract void generateSelect(StringBuffer cmd);
    protected abstract void generateFrom(StringBuffer cmd);
    protected abstract void generateWhere(StringBuffer cmd);
    protected abstract void generateGroup(StringBuffer cmd);
    protected abstract void generateOrder(StringBuffer cmd);


    protected void makeSelect(StringBuffer cmd, String[] tables,
            String[] columns, String[] aliases) {

        cmd.append(SELECT + ' ');

        for (int i = 0;  i < tables.length && i < columns.length
                && i < aliases.length;  i++) {

            if (i != 0)
                cmd.append(", ");

            if (tables[i] != null)
                cmd.append('`' + tables[i] + "`.");

            cmd.append('`' + columns[i] + "`");

            if (aliases[i] != null)
                cmd.append(' ' + AS + ' ' + aliases[i]);
        }
    }
}
