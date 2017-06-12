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
    class InputVideoStream : IRandomAccessStream
    {
        private ulong position; // Current position in the stream.
        private ulong size;
        StreamSocket socket;
        private HostName serverHost;
        private string serverPort;

        public InputVideoStream(string serverAddress, string _serverPort)
        {            socket = new StreamSocket();
            //Create a new hostname for the given server address
            serverHost = new HostName(serverAddress);
            serverPort = _serverPort;
            //Connect to the server
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
            }
        }

        public IRandomAccessStream CloneStream()
        {
            throw new NotImplementedException();
        }

        public bool CanRead { get { return true; }  }

        public bool CanWrite { get { return false; } }

        public ulong Position { get { return position; } }

        public ulong Size
        {
            get { return size; }
            set { throw new NotSupportedException(); }
        }

        public IAsyncOperationWithProgress<IBuffer, uint> ReadAsync(IBuffer buffer, uint count, InputStreamOptions options)
        {
            //https://blogs.msdn.microsoft.com/mcsuksoldev/2014/07/12/video-streaming-with-a-custom-irandomaccessstream-on-windows-and-windows-phone-universal-app/

            return AsyncInfo.Run<IBuffer, uint>(async (cancellationToken, progress) =>
            {       
                return await socket.InputStream.ReadAsync(buffer, count, options).AsTask(cancellationToken, progress);
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


        public async Task Connect()
        {
            await socket.ConnectAsync(serverHost, serverPort);
            //Send identifier
            //TODO: should be a json containing some connection identifiers
            IBuffer buffUTF8 = CryptographicBuffer.ConvertStringToBinary("Receiver", BinaryStringEncoding.Utf8);
            //TODO await token??
            await socket.OutputStream.WriteAsync(buffUTF8);
        }
    }
}
