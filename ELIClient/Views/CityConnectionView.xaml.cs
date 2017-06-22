using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace ELIClient.Views
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class CityConnectionView : Page
    {
        RandomAccessStreamReference image = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/map_pointer.png"));
        BitmapImage bitmapImage = new BitmapImage(new Uri("ms-appx:///Assets/map_pointer.png"));


        public CityConnectionView()
        {
            this.InitializeComponent();

            ELIMapControl.MapServiceToken = "14nlfB21xYwkKDaXz3i4~fLTya98uTx2F5NhwwG9MkQ~Ai4rTfaFbUy5F5wCIbYVxQ3TD1iyWBFwBj603L2iXQ3Ti4a1JCJqRvxfl9rRmc52";


            GetAndSetCityPointers();

        }

        /// <summary>
        /// Set the maps view to the users location, if location tracking is enabled.
        /// </summary>
        private async void SetMapLocations()
        {
            var accessStatus = await Geolocator.RequestAccessAsync();
            if (accessStatus == GeolocationAccessStatus.Allowed)
            {                
                    // Get the current location.
                    Geolocator geolocator = new Geolocator();
                    Geoposition pos = await geolocator.GetGeopositionAsync();
                    Geopoint myLocation = pos.Coordinate.Point;

                    // Set the map location.
                    ELIMapControl.Center = myLocation;
                    ELIMapControl.ZoomLevel = 12;
                    ELIMapControl.LandmarksVisible = true;
                               
            }
        }

        private async Task GetAndSetCityPointers()
        {           
            var serverConnector = App.serverConnector;
            await serverConnector.SendGetCitiesRequestAsync();
            var cities = await serverConnector.ReceiveMessage();

            var array = cities.GetNamedArray("RESULTS");
            foreach(var item in array)
            {
                var cityObject = item.GetObject();
                var lat = float.Parse(cityObject.GetNamedString("LAT").Replace(',', '.')) ;
                var lon = float.Parse(cityObject.GetNamedString("LON").Replace(',', '.'));
                var name = cityObject.GetNamedString("CITY");
                SetPointer(lat, lon, name);
            }            
        }

        private void SetPointer(float lat, float lon, String name)
        {
            // Create a new button with the tag name
            Button button = new Button
            {
                Content = new Image
                {
                    Source = bitmapImage,
                    VerticalAlignment = VerticalAlignment.Center,
                    HorizontalAlignment = HorizontalAlignment.Center
                },
                BorderThickness = new Thickness(0),
                Tag = name,
                Padding = new Thickness(0),
                Background = new SolidColorBrush(Color.FromArgb(0, 0, 0, 0))                
        };
            
            button.Click += OnPointerClick;

            // Specify a known location.
            BasicGeoposition snPosition = new BasicGeoposition()
            {
                Latitude = lat,
                Longitude = lon
            };
            Geopoint snPoint = new Geopoint(snPosition);
            
            ELIMapControl.Children.Add(button);
            // Set the location on the map
            MapControl.SetLocation(button, snPoint);
            MapControl.SetNormalizedAnchorPoint(button, new Point(0.5, 0.5));
        }

        private void OnPointerClick(object sender, RoutedEventArgs e)
        {
            var button = sender as Button;
            var value = button.Tag;
            CityNamTxtBox.Text = (String) value;
        }
    }

}
