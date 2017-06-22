using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Foundation;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;

namespace ELIClient.SimpleELIServerCommunication
{
    /// <summary>
    /// This class is used for connection to the ELIServer.
    /// Messages can be send and received using an instance of this class.
    /// Every app should have just one ServerConnector instance.
    /// </summary>
    public class ServerConnector
    {
        private StreamSocket socket; //!< The socket to send messages from and receive messages from.
        private HostName serverHost; //!< The hostname of the server
        private String serverPort; //!< The port of the server
        private bool isConnected = false; //!< A boolean indicating that the socket is connected.

        public ServerConnector(String hostname, String port)
        {
            socket = new StreamSocket();
            //Create a new hostname for the given server address
            serverHost = new HostName(hostname);
            serverPort = port;
        }

        public StreamSocket GetSocket()
        {
            return socket;
        }

        /// <summary>
        /// Connect to the ELIServer. 
        /// After the connetion is made, a message with the ClientID and ID of the user is send.
        /// </summary>
        /// <returns>A Task that can be awaited.</returns>
        private async Task Connect()
        {
            await socket.ConnectAsync(serverHost, serverPort);
            isConnected = true;
            JsonObject json = new JsonObject();
            json.Add("ClientID", JsonValue.CreateStringValue("1"));
            json.Add("ID", JsonValue.CreateStringValue("1"));
            json.Add("TYPE", JsonValue.CreateStringValue("Connect"));
            await SendMessage(json);
        }

        /// <summary>
        /// Send a message to the server.
        /// This message is specified in the JsonObject message.
        /// The byteorder of the send message is LittleEndian and the encoding is Utf8.
        /// </summary>
        /// <param name="message">The message to be send.</param>
        /// <returns>A boolean that indicates that the message was succesfully send.</returns>
        public async Task<bool> SendMessage(JsonObject message)
        {
            //Check if the connector is connected. If not, call the Connect() function.
            if (!isConnected)
            {
                await Connect();
            }

            //Write the message to the server.
            //Code used from https://docs.microsoft.com/en-us/uwp/api/windows.storage.streams.datawriter
            try
            {            
                using (var dataWriter = new Windows.Storage.Streams.DataWriter(socket.OutputStream))
                {
                    dataWriter.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
                    dataWriter.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;

                    // Parse the input stream and write each element separately.
                    //Get the string version of the json object.
                    var stringToSend = message.ToString();

                    //Get the length of the stream, so the server knows how much data was send.
                    uint inputElementSize = dataWriter.MeasureString(stringToSend);
                    dataWriter.WriteUInt32(inputElementSize);
                    dataWriter.WriteString(stringToSend);

                    Debug.WriteLine("Wrote" + stringToSend);
                    Debug.WriteLine("Lenght:" + inputElementSize);

                    // Send the contents of the writer to the backing stream.
                    await dataWriter.StoreAsync();

                    // For the in-memory stream implementation we are using, the flushAsync call 
                    // is superfluous,but other types of streams may require it.
                    await dataWriter.FlushAsync();

                    // In order to prolong the lifetime of the stream, detach it from the 
                    // DataWriter so that it will not be closed when Dispose() is called on 
                    // dataWriter. Were we to fail to detach the stream, the call to 
                    // dataWriter.Dispose() would close the underlying stream, preventing 
                    // its subsequent use by the DataReader below.
                    dataWriter.DetachStream();
                    return true;
                }
            }
            catch
            {
                //TODO on error?
                throw;
            }
        }

        /// <summary>
        /// Send a GetRandomTable request to the server.
        /// </summary>
        /// <returns>A Task that can be awaited.</returns>
        public async Task SendRandomConnectionRequestMessageAsync()
        {
            JsonObject json = new JsonObject();
            json.Add("ClientID", JsonValue.CreateStringValue("1"));
            json.Add("ID", JsonValue.CreateStringValue("1"));
            json.Add("TYPE", JsonValue.CreateStringValue("GetRandomTable"));
            await SendMessage(json);
        }

        /// <summary>
        /// Send a GetAllCities request to the server.
        /// </summary>
        /// <returns>A Task that can awaited.</returns>
        public async Task SendGetCitiesRequestAsync()
        {
            JsonObject json = new JsonObject();
            json.Add("ClientID", JsonValue.CreateStringValue("1"));
            json.Add("ID", JsonValue.CreateStringValue("1"));
            json.Add("TYPE", JsonValue.CreateStringValue("GetAllCities"));
            await SendMessage(json);
        }

        /// <summary>
        /// Receive a message response from the server.
        /// The encoding of the message should be Utf8 and the byteOrder should be LittleEndian.
        /// The message is parsed to a JsonObject using JsonObject.Parse() method.
        /// </summary>
        /// <returns>The message in JsonObject format.</returns>
        public async Task<JsonObject> ReceiveMessage()
        {
            //Check if the connector is connected. If not, call the Connect() function.
            if (!isConnected)
            {
                await Connect();
            }

            var readString = "";
            using (DataReader dataReader = new DataReader(socket.InputStream))
            {
                dataReader.InputStreamOptions = InputStreamOptions.Partial;
                dataReader.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
                dataReader.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;

                // Load first 4 bytes that represent the size of the following message.
                await dataReader.LoadAsync(4);
                var size = dataReader.ReadUInt32();
                // Read the message.
                await dataReader.LoadAsync(size);
                readString = dataReader.ReadString(size);

                // Detach stream so that it won't be closed when the datareader is disposed later
                dataReader.DetachStream();
            }

            Debug.WriteLine(readString);

            return JsonObject.Parse(readString);
        }      
    }
}
