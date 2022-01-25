using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using ArduinoSensors.Models;


namespace ArduinoSensors.Controllers
{
    [ApiController]
    [Route("[controller]")]

    public class SensorDataController : ControllerBase
    {
        readonly DataContext Context;
        public SensorDataController(DataContext context)
            => Context = context;

        [HttpPost]
        public IActionResult Create(SensorData sensorData)
        {
            sensorData.Timestamp = DateTime.Now;

            Context.Add(sensorData);
            Context.SaveChanges();

            return Created("192.168.0.77/SensorAPI/SensorData/", sensorData);
        }
    }
}
