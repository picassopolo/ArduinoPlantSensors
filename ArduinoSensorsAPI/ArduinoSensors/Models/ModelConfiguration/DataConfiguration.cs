using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;

namespace ArduinoSensors.Models.ModelConfiguration
{
    public class DataConfiguration : IEntityTypeConfiguration<SensorData>
    {
        public void Configure(EntityTypeBuilder<SensorData> builder)
        {
            builder.HasKey(prop => prop.Id);

            builder.Property(prop => prop.Temperature);
            builder.Property(prop => prop.Light);
                
        }
    }
}
