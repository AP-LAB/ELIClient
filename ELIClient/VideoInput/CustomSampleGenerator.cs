using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.Storage.Streams;

namespace ELIClient
{
    class CustomSampleGenerator
    {
        public MediaStreamSample GetSample(MediaStreamSourceSampleRequest request)
        {
           
            IBuffer i = new Windows.Storage.Streams.Buffer(2);
            //MediaStreamSample.CreateFromBuffer(i, );

            
            return null;

        }
    }
}
