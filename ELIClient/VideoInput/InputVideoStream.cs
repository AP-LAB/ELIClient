using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;

namespace ELIClient
{
    /// <summary>
    /// This class is used as an input stream for a MediaElement.
    /// The instance reads video data from the server.
    /// 
    /// Note: this implementation of the class is not working correctly yet.
    /// </summary>
    class InputVideoStream : IRandomAccessStream
    {
        private ulong position; //!< Current position in the stream.
        private ulong size = ulong.MaxValue; //!< The size of the stream.
        StreamSocket socket; //!< The socket to stream video data from.
        private HostName serverHost; //!< The server hostname.
        private string serverPort; //!< The server port.
        private bool connected = false; //!< A boolean indicating the state of the socket.

        /// <summary>
        /// This constructor is used for testing purposes.
        /// Use the InputVideoStream(StreamSocket) instead if this constructor.
        /// </summary>
        /// <param name="serverAddress">The address of the server.</param>
        /// <param name="_serverPort">The port of the server.</param>
        public InputVideoStream(string serverAddress, string _serverPort)
        {
            socket = new StreamSocket();
            //Create a new hostname for the given server address
            serverHost = new HostName(serverAddress);
            serverPort = _serverPort;
        }

        public InputVideoStream(StreamSocket _socket)
        {
            socket = _socket;
        }

        public IInputStream GetInputStreamAt(ulong position)
        {
            throw new NotImplementedException();
        }

        public IOutputStream GetOutputStreamAt(ulong position)
        {
            throw new NotImplementedException();
        }

        public void Seek(ulong _position)
        {
            position = _position;

            if (position >= size)
            {
                size = position + 1;
                Debug.WriteLine("newSize " + size);
            }
        }

        public IRandomAccessStream CloneStream()
        {
            throw new NotImplementedException();
        }

        public bool CanRead { get { return true; }  }

        public bool CanWrite { get { return false; } }

        public ulong Position {
            get {
                Debug.WriteLine("Got position: " + position);
                return position;              
            }
        }

        public ulong Size
        {
            get
            {
                Debug.WriteLine("Got size: " + size);
                return size; }
            set { throw new NotSupportedException(); }
        }

        public IAsyncOperationWithProgress<IBuffer, uint> ReadAsync(IBuffer buffer, uint count, InputStreamOptions options)
        {
            return AsyncInfo.Run<IBuffer, uint>(async (cancellationToken, progress) =>
            {
                //Wait unil the socket is connected
                while (!connected)
                {
                    await Task.Delay(TimeSpan.FromMilliseconds(1));
                }
                
                IBuffer newBuffer = await socket.InputStream.ReadAsync(buffer, count, options).AsTask(cancellationToken, progress);

                position += newBuffer.Length;
                return newBuffer;

            });
        }

        public IAsyncOperationWithProgress<uint, uint> WriteAsync(IBuffer buffer)
        {            
            throw new NotImplementedException();
        }

        public IAsyncOperation<bool> FlushAsync()
        {
            throw new NotImplementedException();
        }

        public void Dispose()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// This method is needed in combination with the StreamSocket(serverhost, serverport) constructor.
        /// </summary>
        /// <returns>A task that can be awaited.</returns>
        public async Task Connect()
        {
            await socket.ConnectAsync(serverHost, serverPort);
            //Send identifier
            //TODO: should be a json containing some connection identifiers
            IBuffer buffUTF8 = CryptographicBuffer.ConvertStringToBinary("Receiver", BinaryStringEncoding.Utf8);
            //TODO await token??
            await socket.OutputStream.WriteAsync(buffUTF8);

            Debug.WriteLine("Socket address: " + socket.Information.RemoteAddress + " And " + socket.Information.LocalAddress);
            connected = true;
        }
    }
}
