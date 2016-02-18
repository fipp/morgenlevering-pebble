/*
var myAPIKey = '';

function iconFromWeatherId(weatherId) {
  if (weatherId < 600) {
    return 2;
  } else if (weatherId < 700) {
    return 3;
  } else if (weatherId > 800) {
    return 1;
  } else {
    return 0;
  }
}
*/

var userId = 406497;
var products, activeProductIndex;

function fetchUserProducts () {
  var req = new XMLHttpRequest();
  req.open('GET', 'http://dev-subscribe.di.no/api/pebble/products/' + userId, true);
  req.onload = function () {
    console.log('req.readyState ' + req.readyState);
    if (req.readyState === 4) {
      if (req.status === 200) {
        products = JSON.parse(req.responseText);
        if (products.length > 0) {
          activeProductIndex = 0;
          sendAppMessage(products[activeProductIndex]);
        }
        //sendAppMessage(response, response.length);
      } else {
        console.log('Error');
      }
    }
  };
  req.send(null);
}

function orderProduct () {
  var req = new XMLHttpRequest();
  var orderUrl = 'http://10.200.197.112:8080/api/pebble/order/' + userId + '/' + products[activeProductIndex].externalId;
  console.log('POSTing URL ' + orderUrl);
  req.open('POST', orderUrl, true);
  req.onload = function () {
    console.log('onload ... readyState: ' + req.readyState);
    if (req.readyState === 4) {
      console.log('req.status: ' + req.status);
      if (req.status === 200) {
        console.log('Great success!');
        Pebble.sendAppMessage({'orderStatus': 'Bestilt!'});
      } else {
        console.log('Fail!');
        Pebble.sendAppMessage({'orderStatus': 'Bestilling feilet!'});
      }
    }
  };
  req.send(null);
}

//function sendAppMessage (messages, index) {
//  console.log('Sending message ' + index + ' ...');
function sendAppMessage(message) {
//  Pebble.sendAppMessage(messages[index],
  Pebble.sendAppMessage(message,
    function(e) {
      /*
      var nextIndex = index - 1;
      if (nextIndex > 0) {
        sendAppMessage(messages, nextIndex)  
      }
      */
      console.log('Successfully delivered message with transactionId=' + e.data.transactionId);
    },
    function(e) {
      console.log('Unable to deliver message with transactionId=' + e.data.transactionId + ' Error is: ' + e.error.message);
    }
  );
}

/*
function fetchWeather(latitude, longitude) {
  var req = new XMLHttpRequest();
  
  req.open('GET', 'http://api.openweathermap.org/data/2.5/weather?' +
    'lat=' + latitude + '&lon=' + longitude + '&cnt=1&appid=' + myAPIKey, true);
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        console.log(req.responseText);
        var response = JSON.parse(req.responseText);
        var temperature = Math.round(response.main.temp - 273.15);
        var icon = iconFromWeatherId(response.weather[0].id);
        var city = response.name;
        console.log(temperature);
        console.log(icon);
        console.log(city);
        Pebble.sendAppMessage({
          'WEATHER_ICON_KEY': icon,
          'WEATHER_TEMPERATURE_KEY': temperature + '\xB0C',
          'WEATHER_CITY_KEY': city
        });
      } else {
        console.log('Error');
      }
    }
  };
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    'WEATHER_CITY_KEY': 'Loc Unavailable',
    'WEATHER_TEMPERATURE_KEY': 'N/A'
  });
}

var locationOptions = {
  'timeout': 15000,
  'maximumAge': 60000
};
*/

Pebble.addEventListener('ready', function (e) {
  console.log("ready...");
  fetchUserProducts();
});

Pebble.addEventListener('appmessage', function (e) {
  console.log('appmessage received: ' + Object.keys(e.payload).join(','));
  if ('getNextProduct' in e.payload && activeProductIndex < products.length - 1) {
    console.log('getting next product ...');
    activeProductIndex += 1;
    sendAppMessage(products[activeProductIndex]);
  } else if ('getPreviousProduct' in e.payload && activeProductIndex > 0) {
    console.log('getting previous product ...');
    activeProductIndex -= 1;
    sendAppMessage(products[activeProductIndex]);
  } else if ('orderProduct' in e.payload) {
    console.log('ordering product ...');
    orderProduct();
  }
  
  
  
  
  /*
  console.log("addmessage js");
  console.log("        payload: " + e.payload);
  console.log("           type: " + e.payload.constructor.name);
  var requestPayloadAsJsonString = JSON.stringify (e.payload);
  console.log("    stringified: " + requestPayloadAsJsonString);
  console.log("products");
  console.log(products);
  console.log(activeProductIndex);
  */
});

Pebble.addEventListener('webviewclosed', function (e) {
  console.log('webview closed');
  console.log(e.type);
  console.log(e.response);
});

/*
Pebble.addEventListener('ready', function (e) {
  console.log('connect!' + e.ready);
  window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
    locationOptions);
  console.log(e.type);
});

Pebble.addEventListener('appmessage', function (e) {
  window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
    locationOptions);
  console.log(e.type);
  console.log(e.payload.temperature);
  console.log('message!');
});

Pebble.addEventListener('webviewclosed', function (e) {
  console.log('webview closed');
  console.log(e.type);
  console.log(e.response);
});
*/