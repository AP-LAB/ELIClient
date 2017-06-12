using System;
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
    class Streamer
    {
        private StreamSocket socket;
        private HostName serverHost;
        private string serverPort;

        public Streamer(string serverAddress, string _serverPort)
        {
            socket = new StreamSocket();
            //Create a new hostname for the given server address
            serverHost = new HostName(serverAddress);
            serverPort = _serverPort;
            //Connect to the server
        }

        //Connect to the given server
        //TODO do checks
        public async Task Connect()
        {
            await socket.ConnectAsync(serverHost, serverPort);
            //Send identifier
            //TODO: should be a json containing some connection identifiers
            IBuffer buffUTF8 = CryptographicBuffer.ConvertStringToBinary("Sender", BinaryStringEncoding.Utf8);

            //TODO await token??
            await socket.OutputStream.WriteAsync(buffUTF8);

            //var buffer = new Windows.Storage.Streams.Buffer(1024);
            //await socket.InputStream.ReadAsync(buffer, buffer.Capacity, InputStreamOptions.Partial); 




        }

        //Write the received data to the connected socket
        //TODO: do checks
        public async void WriteToSocket(IBuffer buffer)
        {

            try
            {
               await socket.OutputStream.WriteAsync(buffer);

                
            }
            catch (Exception ex)
            {
                Debug.WriteLine("WriteToSocket() OutputVideoStream error: ");
                Debug.WriteLine(ex.Message);
                Debug.WriteLine(ex.Source);
                //TODO what if it went wrong??
            }
        }

        internal void CloseConnection()
        {
            socket.Dispose();
        }
    }

    
}
