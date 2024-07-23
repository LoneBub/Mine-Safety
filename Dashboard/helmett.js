// Data arrays to hold the fetched values
var tempData = [0];
var humidityData = [0];
var angleData = [0];

// Function to add data points and manage data length
function addDataPoint(TD, HD, AD) {
  tempData.push(TD);
  humidityData.push(HD);
  angleData.push(AD);
  if (tempData.length > 5) {
    tempData.shift();
  }
  if (humidityData.length > 5) {
    humidityData.shift();
  }
  if (angleData.length > 5) {
    angleData.shift();
  }
}

// Function to fetch data and update the chart
async function fetchData() {
  try {
    let response = await fetch("http://localhost:8000/data1.json");
    if (response.ok) {
      let jsonData = await response.json();
      let temp = jsonData.Temp;
      let humidity = jsonData.Humidity;
      let angle = jsonData.angle;
      addDataPoint(temp, humidity, angle);

      const now = new Date();
      const timeLabel = `${now.getHours()}:${now.getMinutes()}:${now.getSeconds()}`;

      data.labels.push(timeLabel);
      data.datasets[0].data.push(temp);
      data.datasets[1].data.push(humidity);
      data.datasets[2].data.push(angle);

      // Update chart data
      if (data.labels.length >= 5) {
        data.labels.shift();
        data.datasets[0].data.shift();
        data.datasets[1].data.shift();
        data.datasets[2].data.shift();
      }

      // Update the chart with new data
      myChart.update();
      console.log("temperature : ", tempdata);
      console.log("humidity : ", humiditydata);
      console.log("angle : ", angledata);
    } else {
      console.error("Network response was not ok.");
    }
  } catch (error) {
    console.error("Fetch error:", error);
  }
}

// Initial chart data and configuration
const data = {
  labels: [], // Start with an empty array for labels
  datasets: [
    {
      label: "Temp",
      backgroundColor: "rgba(255, 99, 132, 0.2)",
      borderColor: "rgba(255, 99, 132, 1)",
      borderWidth: 1,
      data: [], // Start with an empty array for data
      fill: false,
    },
    {
      label: "Humidity",
      backgroundColor: "rgba(54, 162, 235, 0.2)",
      borderColor: "rgba(54, 162, 235, 1)",
      borderWidth: 1,
      data: [], // Start with an empty array for data
      fill: false,
    },
    {
      label: "Angle",
      backgroundColor: "rgba(75, 192, 192, 0.2)",
      borderColor: "rgba(75, 192, 192, 1)",
      borderWidth: 1,
      data: [], // Start with an empty array for data
      fill: false,
    },
  ],
};

// Chart configuration
const config = {
  type: "line",
  data: data,
  options: {
    maintainAspectRatio: false,
    scales: {
      y: {
        beginAtZero: true,
      },
    },
  },
};

// Create the chart (only once)
const myChart = new Chart(
  document.getElementById("myChart").getContext("2d"),
  config
);

// Fetch data initially and then every 2 seconds
fetchData();
window.setInterval(fetchData, 2000);
