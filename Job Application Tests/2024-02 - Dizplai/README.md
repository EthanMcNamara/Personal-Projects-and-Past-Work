# Dizplai Tech Test
This Project was completed as part of my application as a Graduate Software Engineer at Dizplai.\
The premise of this project was to make a simple web app that allows users to vote on a poll and view the results of said poll.

## Technology used
This app was designed on a Windows device using node.js, with a simple server and API via express.js, as well as a MongoDB Database, with unit tests run through Jest.\
I was somewhat unfamiliar with a handful of the requirements in this project, and as such chose the above technologies due to their range of documentation, ease of understanding and strong community support to assist me when I needed to understand required concepts for this project.

## Setup
### Tech required:
Due to the technology used, it will be necessary to have node.js and MongoDB installed to run this app.\
node.js can be installed through NVM on [Windows](https://github.com/coreybutler/nvm-windows), or on [Mac and Linux](https://github.com/nvm-sh/nvm).\
MongoDB can be installed for free via the [Community version](https://www.mongodb.com/try/download/community)

### Installing node.js
Once NVM is installed, open your terminal and enter:
```
nvm install latest
```

### Setting up MongoDB
Upon opening MongoDB, set your URI to:
```
mongodb://localhost:27017
```
This should be the default value. Then press "connect".

## Running the Server
Once Node.js and MongoDB have been installed and setup, open the root folder of the project and enter the following into your terminal:
```
npm run start
```
Once the console has listed "`Listening on port 3000`", the server is active.\
The main website can be accessed by opening `localhost:3000` in your browser.\
Once the server is active, unit tests can be run by entering the following into a new Terminal window:
```
npm run test
```
