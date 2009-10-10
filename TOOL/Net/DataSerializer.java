
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

package TOOL.Net;

import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.StreamCorruptedException;

import TOOL.TOOL;

public class DataSerializer {

    public static final int UDP_PORT = 4000;
    public static final int TCP_PORT = 4001;
    public static final int BUF_SIZE = 1048576; // 1 MB - seems large, but one
                                                // Nao image is 614400

    public static final int SIZEOF_INT = 4;
    public static final int SIZEOF_BYTE = 1;
    public static final int SIZEOF_FLOAT = SIZEOF_INT;
    public static final int SIZEOF_LONG = 8;
    public static final int SIZEOF_DOUBLE = SIZEOF_LONG;

    public static final byte TYPE_INT  = 0;
    public static final byte TYPE_BYTE  = 1;
    public static final byte TYPE_FLOAT  = 2;
    public static final byte TYPE_LONG    = 3;
    public static final byte TYPE_DOUBLE   = 4;    // this is so cool
    public static final byte TYPE_INT_ARRAY = 5;
    public static final byte TYPE_BYTE_ARRAY = 6;
    public static final byte TYPE_FLOAT_ARRAY = 7;
    public static final byte TYPE_LONG_ARRAY   = 8;
    public static final byte TYPE_DOUBLE_ARRAY  = 9;

    //
    // Default Error messages
    //
    public static final String ERROR_NO_INPUT = "Input unavailable on socket.";
    public static final String ERROR_NO_OUTPUT =
        "Output unavailable on socket.";
    public static final String ERROR_DATATYPE = "Unexpected data type read " +
        "from socket connection.";
    public static final String ERROR_DATASIZE = "Unexpected amount of data " +
        "read in array transfer from socket connection.";

    private InetAddress host;
    private Socket sock;
    private DataInputStream input;
    private DataOutputStream output;
    private boolean flushing;

    public DataSerializer(InetAddress remoteHost) {
        host = remoteHost;

        flushing = false;
    }

    public void connect() throws IOException {
        sock = new Socket(host, TCP_PORT);
        input = new DataInputStream(
            new BufferedInputStream(sock.getInputStream(), BUF_SIZE));
        output = new DataOutputStream(
            new BufferedOutputStream(sock.getOutputStream(), BUF_SIZE));
    }

    public void close() {
        try {
            if (sock != null)
                sock.close();
        }catch (IOException e) {
            TOOL.CONSOLE.error("An error occured on disconnect from " + host);
        }
    }

    public void setFlushing(boolean toFlush) {
        flushing = toFlush;
    }

    public void flush() throws IOException {
        output.flush();
    }

    //
    // Stream writing methods
    //

    /**
     * Helper method to automate writing the correct header information for a
     * serialized array, of any type.
     */
    private void writeArrayHeader(byte type, int length) throws IOException {
        if (output == null)
            throw new IOException(ERROR_NO_OUTPUT);

        output.write(type);
        output.writeInt(length);
    }

    /**
     *
     */
    public synchronized void writeInt(int value) throws IOException {
        if (output == null)
            throw new IOException(ERROR_NO_OUTPUT);

        output.write(TYPE_INT);
        output.writeInt(value);

        if (flushing)
            output.flush();
    }

    /**
     *
     */
    public synchronized void writeByte(byte value) throws IOException {
        if (output == null)
            throw new IOException(ERROR_NO_OUTPUT);

        output.write(TYPE_BYTE);
        output.write(value);

        if (flushing)
            output.flush();
    }

    public synchronized void writeByte(boolean value) throws IOException {
        if (value)
            writeByte((byte)1);
        else
            writeByte((byte)0);
    }

    public synchronized void writeFloat(float value) throws IOException {
        if (output == null)
            throw new IOException(ERROR_NO_OUTPUT);

        output.write(TYPE_FLOAT);
        output.writeFloat(value);

        if (flushing)
            output.flush();
    }

    /**
     * Serialize and write an integer array to the output stream.  Prepends the
     * message with the standard array header giving the data type and length.
     */
    public synchronized void writeInts(int[] data) throws IOException {
        writeArrayHeader(TYPE_INT_ARRAY, data.length * SIZEOF_INT);

        for (int i = 0; i < data.length; i++)
            output.writeInt(data[i]);

        if (flushing)
            output.flush();
    }

    /**
     * Serialize and write a two dimensional integer array to the output
     * stream.  Prepends the message with the standard array header giving the
     * data type and length.
     */
    public synchronized void writeInts(int[][] data) throws IOException {
        writeArrayHeader(TYPE_INT_ARRAY,
            data.length * data[0].length * SIZEOF_INT);

        for (int i = 0; i < data.length; i++)
            for (int j = 0; j < data[0].length; j++)
                output.writeInt(data[i][j]);

        if (flushing)
            output.flush();
    }

    /**
     * Serialize and write a float array to the output stream.  Prepends the
     * message with the standard array header giving the data type and length.
     */
    public synchronized void writeFloats(float[] data) throws IOException {
        writeArrayHeader(TYPE_FLOAT_ARRAY, data.length * SIZEOF_FLOAT);

        for (int i = 0; i < data.length; i++)
            output.writeFloat(data[i]);

        if (flushing)
            output.flush();
    }

    /**
     * Serialize and write a double array to the output stream.  Prepends the
     * message with the standard array header giving the data type and length.
     */
    public synchronized void writeDoubles(double[] data) throws IOException {
        writeArrayHeader(TYPE_DOUBLE_ARRAY, data.length * SIZEOF_DOUBLE);

        for (int i = 0; i < data.length; i++)
            output.writeDouble(data[i]);

        if (flushing)
            output.flush();
    }

    /**
     * Serialize and write a two dimensional double array to the output
     * stream.  Prepends the message with the standard array header giving the
     * data type and length.
     */
    public synchronized void writeDoubles(double[][] data) throws IOException {
        writeArrayHeader(TYPE_DOUBLE_ARRAY,
            data.length * data[0].length * SIZEOF_DOUBLE);

        for (int i = 0; i < data.length; i++)
            for (int j = 0; j < data[0].length; j++)
                output.writeDouble(data[i][j]);

        if (flushing)
            output.flush();
    }

    /**
     *
     */
    public synchronized void writeBytes(byte[] data) throws IOException {
        writeArrayHeader(TYPE_BYTE_ARRAY, data.length);

        output.write(data);

        if (flushing)
            output.flush();
    }

    public synchronized void writeBytes(boolean[] data) throws IOException {
        writeArrayHeader(TYPE_BYTE_ARRAY, data.length);

        for (int i = 0; i < data.length; i++)
            output.writeBoolean(data[i]);

        if (flushing)
            output.flush();
    }

    /**
     *
     */
    public synchronized void writeBytes(byte[][] data) throws IOException {
        writeArrayHeader(TYPE_BYTE_ARRAY, data.length * data[0].length);

        for (int i = 0; i < data.length; i++)
            output.write(data[i]);

        if (flushing)
            output.flush();
    }

    //
    // Stream reading methods
    //

    /**
     * Helper method to automate reading the expected header information for
     * a serialized array, of any type.
     */
    private void readArrayHeader(byte type, int length) throws IOException {
        if (input == null)
            throw new IOException(ERROR_NO_INPUT);

        byte actualType = input.readByte();
        if (actualType != type)
            errorDataType(type, actualType);

        int actualLength = input.readInt();
        if (actualLength != length)
            errorDataSize(length, actualLength);
    }

    private int readArrayHeader(byte type, int length, boolean variableLength)
            throws IOException {
        if (input == null)
            throw new IOException(ERROR_NO_INPUT);

        byte actualType = input.readByte();
        if (actualType != type)
            errorDataType(type, actualType);

        int actualLength = input.readInt();
        if (variableLength && actualLength > length ||
                !variableLength && actualLength != length)
            errorDataSize(length, actualLength);

        return actualLength;
    }


    /**
     *
     */
    public synchronized int readInt() throws IOException {
        if (input == null)
            throw new IOException(ERROR_NO_INPUT);

        byte actualType = input.readByte();
        if (actualType != TYPE_INT)
            errorDataType(TYPE_INT, actualType);

        return input.readInt();
    }

    /**
     *
     */
    public synchronized byte readByte() throws IOException {
        if (input == null)
            throw new IOException(ERROR_NO_INPUT);

        byte actualType = input.readByte();
        if (actualType != TYPE_BYTE)
            errorDataType(TYPE_BYTE, actualType);

        return input.readByte();
    }

    /**
     * Read a serialized integer array from the input stream.  Expects the
     * message to be prepended with the standard array header giving the
     * correct data type and length.
     */
    public synchronized void readInts(int[] data) throws IOException {
        readArrayHeader(TYPE_INT_ARRAY, data.length * SIZEOF_INT);

        for (int i = 0; i < data.length; i++)
            data[i] = input.readInt();
    }

    public synchronized int readInts(int[] data, boolean variableLength)
            throws IOException {
        int length = readArrayHeader(TYPE_INT_ARRAY, data.length * SIZEOF_INT,
            variableLength) / SIZEOF_INT;

        for (int i = 0; i < length; i++)
            data[i] = input.readInt();
        return length;
    }

    /**
     * Read a serialized two dimensional integer array from the output stream.
     * Expects the message to be prepended with the standard array header
     * giving the correct data type and length.
     */
    public synchronized void readInts(int[][] data) throws IOException {
        readArrayHeader(TYPE_INT_ARRAY,
            data.length * data[0].length * SIZEOF_INT);

        for (int i = 0; i < data.length; i++)
            for (int j = 0; j < data[0].length; j++)
                data[i][j] = input.readInt();
    }

    /**
     * Read a serialized double array from the input stream.  Expects the
     * message to be prepended with the standard array header giving the
     * correct data type and length.
     */
    public synchronized void readFloats(float[] data) throws IOException {
        readArrayHeader(TYPE_FLOAT_ARRAY, data.length * SIZEOF_FLOAT);

        for (int i = 0; i < data.length; i++)
            data[i] = input.readFloat();
    }

    /**
     * Read a serialized double array from the input stream.  Expects the
     * message to be prepended with the standard array header giving the
     * correct data type and length.
     */
    public synchronized void readDoubles(double[] data) throws IOException {
        readArrayHeader(TYPE_DOUBLE_ARRAY, data.length * SIZEOF_DOUBLE);

        for (int i = 0; i < data.length; i++)
            data[i] = input.readDouble();
    }

    /**
     * Read a serialized two dimensional double array from the output stream.
     * Expects the message to be prepended with the standard array header
     * giving the correct data type and length.
     */
    public synchronized void readDoubles(double[][] data) throws IOException {
        readArrayHeader(TYPE_DOUBLE_ARRAY,
            data.length * data[0].length * SIZEOF_DOUBLE);

        for (int i = 0; i < data.length; i++)
            for (int j = 0; j < data[0].length; j++)
                data[i][j] = input.readDouble();
    }

    /**
     *
     */
    public synchronized void readBytes(byte[] data) throws IOException {
        readArrayHeader(TYPE_BYTE_ARRAY, data.length);

        input.readFully(data);
    }

    public synchronized int readBytes(byte[] data, boolean variableLength)
            throws IOException {
        int length = readArrayHeader(TYPE_BYTE_ARRAY, data.length,
            variableLength);

        input.readFully(data, 0, length);
        return length;
    }

    /**
     *
     */
    public synchronized void readBytes(byte[][] data) throws IOException {
        readArrayHeader(TYPE_BYTE_ARRAY, data.length * data[0].length);

        for (int i = 0; i < data.length; i++)
            input.readFully(data[i]);
    }

    private void errorDataSize(int expected, int received) throws IOException {
        throw new IOException(ERROR_DATASIZE + "\n  Expected=" + expected +
            ", received=" + received);
    }

    private void errorDataType(int expected, int received) throws IOException {
        throw new StreamCorruptedException(ERROR_DATATYPE + "\n  Expected=" +
            expected + ", received=" + received);
    }
}
