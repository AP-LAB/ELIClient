using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Runtime.Serialization;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;

namespace ELIClient
{
    /// <summary>
    /// This class is used as an input IRandomAccessStream for the StartRecordToStreamAsync() method of the MediaCapture class.
    /// The WriteAsync() methode writes all incoming data to the Streamer outputStreamer.
    /// The outputStreamer is passed in the constructor.
    /// </summary>
    class OutputVideoStream : IRandomAccessStream
    {
        private Streamer outputStreamer; //!< The streamer that is used to stream the data to.
        private ulong myPosition; //!< Current position in the stream.
        private ulong mySize;     //!< The size of the stream.

        public OutputVideoStream(Streamer streamer)
        {
            outputStreamer = streamer;
        }

        public IInputStream GetInputStreamAt(ulong position)
        {
            throw new NotImplementedException();
        }

        public IOutputStream GetOutputStreamAt(ulong position)
        {
            throw new NotImplementedException();
        }

        public void Seek(ulong position)
        {
            myPosition = position;

            if (myPosition >= mySize)
            {
                mySize = myPosition + 1;
            }
        }

        public IRandomAccessStream CloneStream()
        {
            throw new NotImplementedException();
        }

        //Cannot read
        public bool CanRead { get { return false; } }

        //Can write
        public bool CanWrite { get { return true; } }

        public ulong Position { get { return myPosition; } }

        public ulong Size
        {
            get { return mySize; }
            set { throw new NotSupportedException(); }
        }

        public IAsyncOperationWithProgress<IBuffer, uint> ReadAsync(IBuffer buffer, uint count, InputStreamOptions options)
        {
            throw new NotImplementedException();
        }

        public IAsyncOperationWithProgress<uint, uint> WriteAsync(IBuffer buffer)
        {
            Task<uint> task = new Task<uint>(() =>
            {
                //Set transferedSize
                uint transferedSize = 0;

                try
                {
                    //Write the buffer to the destination point using the streamer                    
                    outputStreamer.WriteToSocket(buffer);
                    
                    //The amount of data transfered is equal to the length of the buffer
                    transferedSize = buffer.Length;
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("WriteAsync() OutputVideoStream error: ");
                    Debug.WriteLine(ex.Message);
                    Debug.WriteLine(ex.Source);
                    //TODO what if it went wrong??
                    throw;
                }

                return transferedSize;
            });
            //Run the task synchronously
            task.RunSynchronously();

            //Create function for return value using the task
            Func<CancellationToken, IProgress<uint>, Task<uint>> taskProvider =
                (token, progress) => task;
            return AsyncInfo.Run<uint, uint>(taskProvider);
        } 

        public IAsyncOperation<bool> FlushAsync()
        {
            throw new NotImplementedException();
        }

        public void Dispose()
        {
            outputStreamer.CloseConnection();
        }
    }
}
