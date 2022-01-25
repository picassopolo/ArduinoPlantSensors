using ArduinoSensors.Models.ModelConfiguration;
using Microsoft.EntityFrameworkCore;

namespace ArduinoSensors.Models
{
    public class DataContext : DbContext
    {
        public DataContext(DbContextOptions<DataContext> options)
            : base(options) 
        {
        }

        //Create the SensorData table
        protected override void OnModelCreating(ModelBuilder modelBuilder) 
            => modelBuilder.ApplyConfiguration(new DataConfiguration());
    }
}
