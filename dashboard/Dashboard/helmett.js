var tempData = [];
var humidityData = [];
var angleData = [];
var labels = [];

// Function to add data points
function addDataPoint(TD, HD, AD) {
  var now = new Date().toLocaleTimeString();
  labels.push(now);
  tempData.push(TD);
  humidityData.push(HD);
  angleData.push(AD);
  if (labels.length > 5) {
    labels.shift();
    tempData.shift();
    humidityData.shift();
    angleData.shift();
  }
}

// Function to fetch and update data from JSON
async function fetchData() {
  try {
    let response = await fetch("http://localhost:8000/data1.json");
    if (response.ok) {
      let jsonData = await response.json();
      var temp = jsonData.Temp;
      var humidity = jsonData.Humidity;
      var angle = jsonData.angle;
      addDataPoint(temp, humidity, angle);

      // Update the charts
      tempChart.data.labels = labels;
      tempChart.data.datasets[0].data = tempData;
      tempChart.update();

      humidityChart.data.labels = labels;
      humidityChart.data.datasets[0].data = humidityData;
      humidityChart.update();

      angleChart.data.labels = labels;
      angleChart.data.datasets[0].data = angleData;
      angleChart.update();

      // Update the current values on the web page
      document.getElementById(
        "tempValue"
      ).innerText = `Current Temperature: ${temp}°C`;
      document.getElementById(
        "humidityValue"
      ).innerText = `Current Humidity: ${humidity}%`;
      document.getElementById(
        "angleValue"
      ).innerText = `Current Angle: ${angle}°`;

      console.log("Temperature: ", tempData);
      console.log("Humidity: ", humidityData);
      console.log("Angle: ", angleData);
    } else {
      console.error("Network response was not ok.");
    }
  } catch (error) {
    console.error("Fetch error:", error);
  }
}

// Initial setup for the charts
const tempCtx = document.getElementById("tempChart").getContext("2d");
const tempChart = new Chart(tempCtx, {
  type: "line",
  data: {
    labels: [],
    datasets: [
      {
        label: "Temp",
        backgroundColor: "rgba(255, 99, 132, 0.2)",
        borderColor: "rgba(255, 99, 132, 1)",
        borderWidth: 1,
        data: [],
        fill: false,
      },
    ],
  },
  options: {
    maintainAspectRatio: false,
    scales: {
      y: {
        beginAtZero: true,
        min: 0,
      },
    },
  },
});

const humidityCtx = document.getElementById("humidityChart").getContext("2d");
const humidityChart = new Chart(humidityCtx, {
  type: "line",
  data: {
    labels: [],
    datasets: [
      {
        label: "Humidity",
        backgroundColor: "rgba(54, 162, 235, 0.2)",
        borderColor: "rgba(54, 162, 235, 1)",
        borderWidth: 1,
        data: [],
        fill: false,
      },
    ],
  },
  options: {
    maintainAspectRatio: false,
    scales: {
      y: {
        beginAtZero: true,
        min: 0,
      },
    },
  },
});

const angleCtx = document.getElementById("angleChart").getContext("2d");
const angleChart = new Chart(angleCtx, {
  type: "line",
  data: {
    labels: [],
    datasets: [
      {
        label: "Angle",
        backgroundColor: "rgba(75, 192, 192, 0.2)",
        borderColor: "rgba(75, 192, 192, 1)",
        borderWidth: 1,
        data: [],
        fill: false,
      },
    ],
  },
  options: {
    maintainAspectRatio: false,
    scales: {
      y: {
        beginAtZero: true,
        min: 0,
      },
    },
  },
});

// Initial fetch and then fetch every 2 seconds
fetchData();
setInterval(fetchData, 2000);
