using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.Media.Playback;
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
            // 
            StartRead();
        }

        public Receiver(StreamSocket _socket)
        {
            socket = _socket;
            //Create a new hostname for the given server address
            //serverHost = new HostName(serverAddress);
            //serverPort = _serverPort;
            ////Connect to the server
            //outputStream = _outputStream;
            // 
            Task.Run(() =>
            {
                StartRead();
            });
        }


        public async Task Connect(HostName serverHost, string serverPort)
        {
            await socket.ConnectAsync(serverHost, serverPort);
            //Send identifier
            //TODO: should be a json containing some connection identifiers
            IBuffer buffUTF8 = CryptographicBuffer.ConvertStringToBinary("Receiver", BinaryStringEncoding.Utf8);
            //TODO await token??
            await socket.OutputStream.WriteAsync(buffUTF8);

            Debug.WriteLine("Socket address: " + socket.Information.RemoteAddress +  " And " + socket.Information.LocalAddress);
        }

        public async void StartRead()
        {

            IInputStream inputStream = socket.InputStream;

            while (true)
            {

                using (DataReader dataReader = new DataReader(inputStream))
                {
                    //dataReader.InputStreamOptions = InputStreamOptions.Partial;
                    dataReader.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;

                    // Load first 4 bytes that represent the size of the following message.
                    await dataReader.LoadAsync(4);
                    var size = dataReader.ReadUInt32();
                    if (size == 0)
                    {
                        // Detach stream so that it won't be closed when the datareader is disposed later
                        dataReader.DetachStream();
                        continue;
                    }

                    // Read the message.
                    await dataReader.LoadAsync(size);
                    IBuffer buffer = dataReader.ReadBuffer(size);

                    // Detach stream so that it won't be closed when the datareader is disposed later
                    dataReader.DetachStream();

                    var source = MediaSource.CreateFromStream(buffer.AsStream().AsRandomAccessStream(), "video/mp4");
                    var item = new MediaPlaybackItem(source);
                    VideoCallView.VideoCallView._playlist.Items.Add(item);
                    VideoCallView.VideoCallView._playlist.MoveNext();
                }


               
                //IBuffer buffer = new Windows.Storage.Streams.Buffer(4);


                //await inputStream.ReadAsync(buffer, buffer.Capacity, InputStreamOptions.Partial);

                //Write the gathered buffer to the output stream
                //await outputStream.WriteAsync(buffer);

                ////TODO receive from server (server part), preview

                //var source = MediaSource.CreateFromStream(buffer.AsStream().AsRandomAccessStream(), "video/mp4");
                //var item = new MediaPlaybackItem(source);
                //VideoCallView.VideoCallView._playlist.Items.Add(item);
                //VideoCallView.VideoCallView._playlist.MoveNext();
            }

        }



    }
}
