import React, {Component} from 'react';
import mqtt from 'mqtt';
import { Line } from 'react-chartjs-2';

import styles from './assets/scss/main.module.scss';

const client = mqtt.connect('ws://broker.hivemq.com:8000/mqtt');


export default class App extends Component {
  constructor() {
    super();

    this.state = {
      data: {
        labels: [],
        datasets: [
          {
            label: 'My First dataset',
            fill: true,
            lineTension: 0.1,
            backgroundColor: 'rgba(75,192,192,0.4)',
            borderColor: 'rgba(75,192,192,1)',
            borderCapStyle: 'butt',
            borderDash: [],
            borderDashOffset: 0.0,
            borderJoinStyle: 'miter',
            pointBorderColor: 'rgba(75,192,192,1)',
            pointBackgroundColor: '#fff',
            pointBorderWidth: 1,
            pointHoverRadius: 5,
            pointHoverBackgroundColor: 'rgba(75,192,192,1)',
            pointHoverBorderColor: 'rgba(220,220,220,1)',
            pointHoverBorderWidth: 2,
            pointRadius: 1,
            pointHitRadius: 10,
            data: []
          }
        ]
      }
    };
  }

  setTemperatures =  msg => {
    const { hmd } = msg;
    const { data } = this.state;

    data.labels.push(data.datasets[0].data.length + 1);
    data.datasets[0].data.push(hmd);
    this.setState(data);
  };

  componentDidMount = () => {
    client.on('connect', () => {
      client.subscribe('/groupname/sensor/tempandhumid');
    });

    client.on('message', (topic, message) => {
      const data = JSON.parse(message);
      switch(topic) {
        case '/groupname/sensor/tempandhumid':
          this.setTemperatures(data);
          break;
        default:
          console.log(`no handler for topic ${topic}}`);
      }
    });
  };

  render() {
    const { data } = this.state;
    console.log(data)
    return (
      <>
        <header>
          <h1>Real time temperature example</h1>
        </header>
        <main>

          <Line
            data={data}
            width={100}
            height={400}
            key={Math.random()}
            options={{
              maintainAspectRatio: false
            }}
            // shouldRedraw={true}
          />
        </main>  
      </>
    )
  }
}
