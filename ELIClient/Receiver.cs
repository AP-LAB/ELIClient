using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;

namespace ELIClient
{
    class Receiver
    {
        StreamSocket socket;
        private HostName serverHost;
        private string serverPort;
        private IRandomAccessStream outputStream;

        public Receiver(string serverAddress, string _serverPort, IRandomAccessStream _outputStream)
        {
            socket = new StreamSocket();
            //Create a new hostname for the given server address
            serverHost = new HostName(serverAddress);
            serverPort = _serverPort;
            //Connect to the server
            outputStream = _outputStream;

            StartRead();
        }

        public async Task Connect(HostName serverHost, string serverPort)
        {
            await socket.ConnectAsync(serverHost, serverPort);
            //Send identifier
            //TODO: should be a json containing some connection identifiers
            IBuffer buffUTF8 = CryptographicBuffer.ConvertStringToBinary("Receiver", BinaryStringEncoding.Utf8);
            //TODO await token??
            await socket.OutputStream.WriteAsync(buffUTF8);
        }

        public async void StartRead()
        {
            await Connect(serverHost, serverPort);

            IInputStream inputStream = socket.InputStream;

            while (true)
            {
                
                Windows.Storage.Streams.Buffer buffer = new Windows.Storage.Streams.Buffer(4096);
                await inputStream.ReadAsync(buffer, buffer.Capacity, InputStreamOptions.Partial);

                //Write the gathered buffer to the output stream
                await outputStream.WriteAsync(buffer);

                //TODO receive from server (server part), preview

                //var source = MediaSource.CreateFromStream(buffer.AsStream().AsRandomAccessStream(), "video/mp4");
                //var item = new MediaPlaybackItem(source);
                //MainPage._playlist.Items.Add(item);
                //MainPage._playlist.MoveNext();


            }
                //TODO detect end https://stackoverflow.com/questions/37259516/how-to-know-whether-the-socket-connection-is-still-alive-when-using-streamsocket
            


        }



    }
}
