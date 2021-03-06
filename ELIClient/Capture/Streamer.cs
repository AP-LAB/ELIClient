﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation.Collections;
using Windows.Media;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;

namespace ELIClient
{
    /// <summary>
    /// This class is used to send data generated by a OutputVideoStream to the server. 
    /// </summary>
    class Streamer
    {
        private StreamSocket socket; //!< The server to write data to.
        private HostName serverHost; //!< The hostname of the server.
        private string serverPort; //!< The port of the server.

        public Streamer(string serverAddress, string _serverPort)
        {
            socket = new StreamSocket();
            //Create a new hostname for the given server address
            serverHost = new HostName(serverAddress);
            serverPort = _serverPort;
            //Connect to the server
        }

        public Streamer(StreamSocket _socket)
        {
            socket = _socket;
        }

        /// <summary>
        /// Connect to the server specified in the constructor.
        /// If the connection fails, an exception will be thrown.
        /// </summary>
        /// <returns>A empty Task.</returns>
        public async Task Connect()
        {
            await socket.ConnectAsync(serverHost, serverPort);
            //Send identifier
            //TODO: should be a json containing some connection identifiers
            IBuffer buffUTF8 = CryptographicBuffer.ConvertStringToBinary("Sender", BinaryStringEncoding.Utf8);
            //TODO await token??
            await socket.OutputStream.WriteAsync(buffUTF8);
        }

        //Write the received data to the connected socket
        //TODO: do checks
        public async void WriteToSocket(IBuffer buffer)
        {
            try
            {
               uint x = await socket.OutputStream.WriteAsync(buffer);            
            }
            catch (Exception ex)
            {
                //Recoring has stopped
                Debug.WriteLine(ex.Message);
                Debug.WriteLine(ex.Source);
                //TODO what if it went wrong??
            }
        }

        public async void WriteToSocketUsingReader(IBuffer buffer)
        {
            using (var dataWriter = new Windows.Storage.Streams.DataWriter(socket.OutputStream))
            {
                dataWriter.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;
                dataWriter.WriteUInt32(buffer.Length);
                dataWriter.WriteBuffer(buffer);
                await dataWriter.StoreAsync();
                await dataWriter.FlushAsync();
                dataWriter.DetachStream();
            }
        }

        internal void CloseConnection()
        {
            socket.Dispose();
        }
    }

    
}
