# -*- coding: utf-8 -*-
"""
Created on Tue May  9 15:54:24 2023

@author: ethmc
"""

import requests
import json
from datetime import datetime
import time
import re

urls = ["https://sc19c2sw.pythonanywhere.com/api/flights/",
        "https://sc19amc.pythonanywhere.com/flights/"]

payeeNames = ["ConAir", "Jolly Roger Travel"]

# Arbitrary payee Ids for each Airline API, detailed in report
payeeIds = [402116, 1074375]
# 'ConAir' = 572, 'sc19c2sw' = 703, 572*703=402116
# 'Jolly Roger Travel' = 1719, sc19amc = 625, 1719*625 = 1074375

bookingUrls = ["https://sc19c2sw.pythonanywhere.com/api/booking/",
        "https://sc19amc.pythonanywhere.com/booking/"]

def logo():
    print("""┌─────────────────────────────────────────────────────────┐
│ ╔═════════════════════════════════════════════════════╗ │
│ ║                                                     ║ │
│ ║     ███████ ██      ██  ██████  ██   ██ ████████    ║ │
│ ║     ██      ██      ██ ██       ██   ██    ██       ║ │
│ ║     █████   ██      ██ ██   ███ ███████    ██       ║ │
│ ║     ██      ██      ██ ██    ██ ██   ██    ██       ║ │
│ ║     ██      ███████ ██  ██████  ██   ██    ██       ║ │
│ ║                                                     ║ │
│ ║                                                     ║ │
│ ║ ██████  ██ ██████   █████  ████████ ███████ ███████ ║ │
│ ║ ██   ██ ██ ██   ██ ██   ██    ██    ██      ██      ║ │
│ ║ ██████  ██ ██████  ███████    ██    █████   ███████ ║ │
│ ║ ██      ██ ██   ██ ██   ██    ██    ██           ██ ║ │
│ ║ ██      ██ ██   ██ ██   ██    ██    ███████ ███████ ║ │
│ ║                                                     ║ │
│ ╚═════════════════════════════════════════════════════╝ │
└─────────────────────────────────────────────────────────┘

Welcome to Flight Pirates Aggregators!""")

def userOptions(status):
    # print description of what user can enter/do
    print("\nPlease enter one of the following commands:")
    
    if(status == 0):
        print(" Search - Begin searching for flights")
        print("   Exit - End the program\n")
        
    if(status == 1):
        print("   [Id] - Select flight matching input Id")
        print(" Cancel - Return to Main Menu\n")
        
    if(status == 2):
        print("   [Id] - Select seat class matching input Id")
        print(" Cancel - Return to Main Menu\n")
    
def searchMenu():
    ext = False
    depAirport = ""
    arrAirport = ""
    urlUsed = 0
    
    validChoice = False
    print("Please enter the number of Passengers, or 'cancel' to return to"+
          " the Main Menu:")
    while(validChoice == False and ext == False):
        passengers = input(">>> ").upper()
        if(passengers == "CANCEL"):
            ext = True
        elif(passengers.isdigit()):
            validChoice = True
        else:
            print("Please ensure the input is a number")
            
    validChoice = False
    if(ext == False):
        print("Please enter the 3-letter code for the Departure Airport, or"+
              " 'Cancel' to return to the Main Menu:")
    
    while(validChoice == False and ext == False):
        depAirport = input(">>> ").upper()
        if(depAirport == "CANCEL"):
            ext = True
        elif(len(depAirport) == 3 and depAirport.isalpha()):
            validChoice = True
        else:
            print("Please ensure the code is only 3 letters long")
            
    validChoice = False
    if(ext == False):
        print("Please enter the 3-letter code for the Arrival Airport, or"+
              " 'Cancel' to return to the Main Menu:")
    
    while(validChoice == False and ext == False):
        arrAirport = input(">>> ").upper()
        if(arrAirport == "CANCEL"):
            ext = True        
        elif(len(arrAirport) == 3 and arrAirport.isalpha()):
            if(depAirport.upper() == arrAirport.upper()):
                print("Departure and Arrival Airport cannot match.")
            else:
                validChoice = True
        else:
            print("Please ensure the code is only 3 letters long")
            
    validChoice = False
    if(ext == False):
        print("Please enter the Date of Departure in YYYY-MM-DD format, or"+
              " 'Cancel' to return to the Main Menu:")
    
    while(validChoice == False and ext == False):
        depDate = input(">>> ").upper()
        dateRegEx = re.compile('....-..-..')
        if(depDate == "CANCEL"):
            ext = True
        elif dateRegEx.match(depDate) is not None:
            # Check each part is an integer
            if(depDate[0:4].isdigit() and depDate[5:7].isdigit() and
               depDate[8:10].isdigit()):
                if(int(depDate[5:7]) > 12):
                    print("Please ensure that YYYY-MM-DD format is used."+
                          " Month must come before Day")
                else:
                    validChoice = True
            else:
                print("Please ensure that only numbers and '-' are used")
        else:
            print("Please ensure that YYYY-MM-DD format is used")
    
    if(ext == False):
        print("\nSearching for Flights...")
        params = {"numPassengers": int(passengers),
                  "departureAirport": depAirport,
                  "arrivalAirport": arrAirport,
                  "departureDate": depDate}
        searchResult = searchQuery(params) # Get all valid search results
        if(isinstance(searchResult, dict) == False): # Returns error
            print("\nNo flights found. Make sure to check the spelling of"+
                  " the Departure and Arrival Airports, as well as the format"+
                  " of the Departure Date")
            ext = True
            
    locs = ""
    date = ""
    times = ""
    duration = ""

    if(ext == False):
        flights = searchResult.values()
        localId = 1
        for flight in flights:
            locs = (flight[1].get('departureAirport')+" - "+
                    flight[1].get('arrivalAirport'))
            times = (flight[1].get('departureDateTime')[11:16]+" - "+
                     flight[1].get('arrivalDateTime')[11:16])
            date = flight[1].get('departureDateTime')[0:10]
            hours = flight[1].get('durationMins') // 60
            minutes = flight[1].get('durationMins') % 60
            duration = str(hours)+"h"+str(minutes)
            price = (str(flight[1].get('minPrice'))+" "+
                     flight[1].get('currency'))
            print("\nId: "+str(localId)+"\n\t"+locs+"\n\t"+date+"\t"+times+" ("+
                  duration+")\n\tFrom "+price+" per person")
            localId += 1
            
    baseUrl = ""
    flightId = 0
    seatId = ""
    currency = ""
    totalCost = 0.0
    costPerTkt = 0.0
    
    cont = False

    if(ext == False):
        userOptions(1)
        
    while (ext == False):
        chosenFlight = input(">>> ")
        if(chosenFlight.isdigit()):
            if chosenFlight in searchResult:
                baseUrl = (searchResult[chosenFlight][0] +
                       str(searchResult[chosenFlight][1].get('flightId')))
                currency = searchResult[chosenFlight][1].get('currency')
                
                flightId = searchResult[chosenFlight][1].get('flightId')
                
                if searchResult[chosenFlight][0] in urls:
                    urlUsed = urls.index(searchResult[chosenFlight][0])
                
                ext = True
                cont = True
            else:
                print("No such Id exists!")
        elif(chosenFlight.lower() == "cancel"):
            ext = True
        else:
            print("This input is not recognised, sorry!")
            
    optUrl = baseUrl + "/options"
    holdUrl = baseUrl + "/hold"
    
    if(cont):
        ext = False
        cont = False
    
    if(ext == False):
        print("\nGetting Seat info...")
        searchResult = getSeats(urlUsed, optUrl, currency) # Get all seats
        if(isinstance(searchResult, dict) == False): # Returns error
            print("\nNo seats found. Flight has been fully booked or there is"+
                  " an error with the Airline Server")
            ext = True
            
    if(ext == False):
        seats = searchResult.values()
        localId = 1
        for seat in seats:
            seatId = str(seat.get('seatClassId'))
            seatClass = seat.get('seatClass')
            seatCost = str(seat.get('price'))+" "+ currency + " per person"
            print("\nId: "+str(localId)+"\n\tClass - "+seatClass+"\n\t"+
                  seatCost)
            localId += 1
       
    chosenClass = ""     
       
    if(ext == False):
        userOptions(2)

    while (ext == False):
        chosenSeat = input(">>> ")
        if(chosenSeat.isdigit()):
            if chosenSeat in searchResult: 
                costPerTkt = float(searchResult[chosenSeat].get('price'))                   
                totalCost = (int(passengers) *
                             float(searchResult[chosenSeat].get('price')))
                seatId = searchResult[chosenSeat].get('seatClassId')
                chosenClass = searchResult[chosenSeat].get('seatClass')
                
                ext = True
                cont = True
            else:
                print("No such Id exists!")
        elif(chosenSeat.lower() == "cancel"):
            ext = True
        else:
            print("This input is not recognised, sorry!")
    
    holdId = 0
    holdSuccessful = False
    
    if(cont):
        ext = False
        cont = False
    
    if(ext == False):
        print("\nReserving seats...")
        holdRequest = {"numPassengers": int(passengers),
                       "seatClassId": seatId}
        r = requests.get(url=holdUrl, params=holdRequest)        
        
        if(r.status_code == 200):
            holdJson = r.json()
            holdId = holdJson.get('holdId')
            holdSuccessful = holdJson.get('holdSuccessful')
            
            if(holdSuccessful == False):
                print("Reservation failed. Please try again or try a different"+
                      " Class")
                ext = True
            else:
                print("Seats reserved!")
        else:
            print("Reservation failed. Please try again or try a different"+
                  " Class")
            ext = True
    
    flightOrderId = 0
    paymentOrderId = 0
    
    if(ext == False):
        # Create Arbitrary Order ID, see report for more details
        timeNow = datetime.now()
        timeId = int(time.mktime(timeNow.timetuple()))
        flightOrderId = int(str(flightId) + str(timeId) + str(holdId))
        
        print("\nCreating Order with Payment system...")
        
        newOrderParams = {"payee_id": payeeIds[urlUsed],
                          "payee_name": payeeNames[urlUsed],
                          "payee_order_id": flightOrderId}
        
        r = requests.post("https://sc19am.pythonanywhere.com/order/new",
                          json=newOrderParams)
        
        if(r.status_code == 200):    
            paymentOrderId = r.json()
            print("Order created!\n")
        else:
            print("Order creation failed. Please try again")
            ext = True
    
    passDetails = []
    
    if(ext == False):        
        for i in range(int(passengers)):
            passengerDetail = createPassengerDetail(i)
            if(isinstance(passengerDetail, bool) and passengerDetail == False):
                ext = True
                break
            else:
                passDetails.append(passengerDetail)
                
    bookingId = 0
    ticketDetails = {}
    seatNumber = ""
    ticketCount = 0
    
    if(ext == False):
        bookingParams = {"flightId": flightId,
                         "holdId": holdId,
                         "seatClassId": seatId,
                         "passengerDetails": passDetails,
                         "paymentInfo": str(paymentOrderId)}
        
        print("\nCreating Passenger Details...")
        r = requests.post(bookingUrls[urlUsed],
                          json=bookingParams)
        
        if(r.status_code == 200):
            bookingDetails = r.json()
            ticketDetails = bookingDetails.get('passengerDetails')
            bookingId = bookingDetails.get('bookingId')
            bookingDate = bookingDetails.get('departureDateTime')
            for ticket in ticketDetails:
                seatNumber = ticket.get('seatNumber')
                
                metadata = {"flight_date": bookingDate,
                            "flight_no": str(flightId),
                            "departure_airport": depAirport,
                            "arrival_airport": arrAirport,
                            "seat": seatNumber}
                
                item = {"item_type": "ticket",
                        "payee_item_id": bookingId,
                        "item_price": costPerTkt,
                        "metadata": metadata}
                
                ticketDetails[ticketCount] = item
                ticketCount += 1
            print("Passenger Details created!")
        else:
            print("Failed to create Passenger Details, please try again.")
            ext = True
            
    deleteUrl = bookingUrls[urlUsed]+str(bookingId)
    
    if(ext == False):
        success = True
        print("\nAdding tickets to Order...")
        for ticket in ticketDetails:
            bodyParam = {"order_id": paymentOrderId,
                         "item": ticket}
            r = requests.post("https://sc19am.pythonanywhere.com/order/add",
                              json=bodyParam)
            if(r.status_code != 200):
                success = False
        if(success):
            print("Tickets added!\n")
        else:
            print("Failed to add tickets. Please try again")
            cancelParams = {"order_id": paymentOrderId,
                            "payment_method_id": 0} # Arbitrary as no paymentMethodId created yet
            r = requests.post("https://sc19am.pythonanywhere.com/order/pay",
                              json=cancelParams)
            ext = True
            
    paymentMethodId = 0
    
    if(ext == False):
        # Display final summary
        print("Flight summary:\n\t"+locs+"\n\t"+date+"\t"+times+" ("+duration+
              ")\n\t"+passengers+"x "+chosenClass+"\n\tTotal Cost: "+
              str(totalCost)+" "+currency)
        
        payMethod = createPaymentmethod()
        if(isinstance(payMethod, bool) and payMethod == False):
            # Cancel booking
            deleteParams = {"bookingId": bookingId}
            r = requests.delete(deleteUrl, json=deleteParams)
            cancelParams = {"order_id": paymentOrderId,
                            "payment_method_id": paymentMethodId}
            r = requests.post("https://sc19am.pythonanywhere.com/order/pay",
                              json=cancelParams)
            ext = True
            print("Booking Cancelled")
        else:
            print("\nCreating new payment method...")
            r = requests.post("https://sc19am.pythonanywhere.com/paymentmethod/new",
                              json=payMethod)
            if(r.status_code == 200):
                print("Payment method created!")
                paymentMethodId = r.json()
            else:
                print("Payment method failed to create. Please try again")
                # Cancel booking
                deleteParams = {"bookingId": bookingId}
                r = requests.delete(deleteUrl, json=deleteParams)
                cancelParams = {"order_id": paymentOrderId,
                            "payment_method_id": paymentMethodId}
                r = requests.post("https://sc19am.pythonanywhere.com/order/pay",
                                json=cancelParams)
                ext = True
    
    # paymentAPI/order/pay, pay for order, if failed then cancel booking
    if(ext == False):
        print("\nPlacing order...")
        payParams = {"order_id": paymentOrderId,
                     "payment_method_id": paymentMethodId}
        r = requests.post("https://sc19am.pythonanywhere.com/order/pay",
                          json=payParams)
        if(r.status_code == 200):
            print("Order complete!\nBooking Ref: "+str(bookingId)+"\nFor more"+
                  " information or to check booking, contact "+
                  payeeNames[urlUsed])
        else:
            print("Order failed. Please try again")
            deleteParams = {"bookingId": bookingId}
            r = requests.delete(deleteUrl, json=deleteParams)
            cancelParams = {"order_id": paymentOrderId,
                            "payment_method_id": paymentMethodId}
            r = requests.post("https://sc19am.pythonanywhere.com/order/pay",
                              json=cancelParams)
        
            
def searchQuery(inParams):
    validFlights = {}
    localId = 1
    
    r = requests.get(url=urls[0], params=inParams) # Connor's API
    if(r.status_code == 200): # On success, process flight details
        flights = r.json()
        for flight in flights:
            validFlights[str(localId)] = [urls[0], flight]
            localId += 1
    
    r = requests.get(url=urls[1], params=inParams) # Alex's API
    if(r.status_code == 200): # On success, process flight details
        flights = r.json()
        for flight in flights['flights']:
            validFlights[str(localId)] = [urls[1], flight]
            localId += 1
    
    if(localId == 1):
        return
    
    return validFlights

def getSeats(urlUsed, optUrl, currency):
    returnSeats = {}
    localId = 1
    
    r = requests.get(url=optUrl)
    if(r.status_code == 200):
        seats = r.json()
        if(urlUsed == 0): # Connor's API
            for seat in seats:
                returnSeats[str(localId)] = seat
                localId += 1
        elif(urlUsed == 1): # Alex
            for seat in seats['seatFares']:
                returnSeats[str(localId)] = seat
                localId += 1
                
    if(localId == 1):
        return
    
    return returnSeats    
    
def createPassengerDetail(i):
    title = ""
    firstName = ""
    surname = ""
    passNum = ""
    dob = ""
    extra = ""
    
    # Prompt user to give passenger details
    validChoice = False
    ext = False
    if(ext == False):
        print("\nPlease enter the title of Passenger "+str(i+1)+", or 'Cancel'"+
              " to return to the Main Menu:")
    while(validChoice == False and ext == False):
        title = input(">>> ").upper()
        if(title == "CANCEL"):
            ext = True
        elif(title.isalpha()):
            validChoice = True
        else:
            print("Please ensure the input exclusively contains"+
                  " letters")
            
    validChoice = False
    if(ext == False):
        print("Please enter the First Name of Passenger "+str(i+1)+", or 'Cancel'"+
              " to return to the Main Menu:")
    while(validChoice == False and ext == False):
        firstName = input(">>> ").upper()
        if(firstName == "CANCEL"):
            ext = True
        elif(firstName.isalpha()):
            validChoice = True
        else:
            print("Please ensure the input exclusively contains"+
                  " letters") 
            
    validChoice = False
    if(ext == False):
        print("Please enter the Surname of Passenger "+str(i+1)+", or 'Cancel'"+
              " to return to the Main Menu:")
    while(validChoice == False and ext == False):
        surname = input(">>> ").upper()
        if(surname == "CANCEL"):
            ext = True
        elif(surname.isalpha()):
            validChoice = True
        else:
            print("Please ensure the input exclusively contains"+
                  " letters")
            
    validChoice = False
    if(ext == False):
        print("Please enter Passenger "+str(i+1)+"'s Passport Number, or 'Cancel'"+
              " to return to the Main Menu:")
    while(validChoice == False and ext == False):
        passNum = input(">>> ").upper()
        if(passNum == "CANCEL"):
            ext = True
        else:
            validChoice = True
            
    validChoice = False
    if(ext == False):
        print("Please enter Passenger "+str(i+1)+"'s Date of Birth in YYYY-MM-DD"+
              " format, or 'Cancel' to return to the Main Menu:")
    while(validChoice == False and ext == False):
        dob = input(">>> ").upper()
        dateRegEx = re.compile('....-..-..')
        if(dob == "CANCEL"):
            ext = True
        elif dateRegEx.match(dob) is not None:
            # Check each part is an integer
            if(dob[0:4].isdigit() and dob[5:7].isdigit() and
               dob[8:10].isdigit()):
                if(int(dob[5:7]) > 12):
                    print("Please ensure that YYYY-MM-DD format is used."+
                          " Month must come before Day")
                else:
                    validChoice = True
            else:
                print("Please ensure that only numbers and '-' are used")
        else:
            print("Please ensure that YYYY-MM-DD format is used")
            
    validChoice = False
    if(ext == False):
        print("Does Passenger "+str(i+1)+" have any additional requirements?\n"+
              "Please enter any requirements here, N/A if there are none or"+
              " 'Cancel' to return to the Main Menu:")
    while(validChoice == False and ext == False):
        extra = input(">>> ").upper()
        if(extra == "CANCEL"):
            ext = True
        else:
            validChoice = True
    
    passengerDetail = {"title": title,
                       "firstName": firstName,
                       "lastName": surname,
                       "passportNumber": passNum,
                       "dateOfBirth": dob,
                       "additionalReq": extra}    
            
    if(ext == True):
        return False
    else:
        return passengerDetail
    
def createPaymentmethod():
    cardNumber = ""
    expiryDate = ""
    cvv = ""
    cardName = ""
    
    # Prompt user to give passenger details
    validChoice = False
    ext = False
    if(ext == False):
        print("\nPlease enter the Payment Card number, or 'Cancel' to return"+
              " to the Main Menu:")
    while(validChoice == False and ext == False):
        cardNumber = input(">>> ").upper()
        if(cardNumber == "CANCEL"):
            ext = True
        elif(cardNumber.isdigit() and len(cardNumber) == 16):
            validChoice = True
        else:
            print("Please ensure the input is 16 digits long and contains"+
                  " only numbers")
            
    validChoice = False
    if(ext == False):
        print("Please enter the Card's expiry date in MM-YY format, or"+
              " 'Cancel' to return to the Main Menu:")
    while(validChoice == False and ext == False):
        date = input(">>> ").upper()
        dateRegEx = re.compile('..-..')
        if(expiryDate == "CANCEL"):
            ext = True
        elif dateRegEx.match(date) is not None:
            # Check each part is an integer
            if(date[0:2].isdigit() and date[3:5].isdigit()):
                if(int(date[0:2]) > 12):
                    print("Please ensure that MM-YY format is used."+
                          " Month must come before Year")
                else:
                    expiryDate = date[0:2]+"/"+date[3:5]
                    validChoice = True
            else:
                print("Please ensure that only numbers and '-' are used")
                print(date[0:2])
                print(date[3:5])
        else:
            print("Please ensure that MM-YY format is used")
    
    validChoice = False
    if(ext == False):
        print("Please enter the Card's CVV, or 'Cancel' to return to the"+
              " Main Menu:")
    while(validChoice == False and ext == False):
        cvv = input(">>> ").upper()
        if(cardNumber == "CANCEL"):
            cvv = True
        elif(cvv.isdigit() and len(cvv) == 3):
            validChoice = True
        else:
            print("Please ensure the input is 3 digits long and contains"+
                  " only numbers")
            
    validChoice = False
    if(ext == False):
        print("Please enter the Name of the cardholder, or 'Cancel' to return"+
              " to the Main Menu:")
    while(validChoice == False and ext == False):
        cardName = input(">>> ").upper()
        if(cardName == "CANCEL"):
            ext = True
        else:
            validChoice = True
    
    paymentmethod = {"card_number": cardNumber,
                     "expiry_date": expiryDate,
                     "cvv": cvv,
                     "cardholder_name": cardName}    
            
    if(ext == True):
        return False
    else:
        return paymentmethod

logo()

ext = False
while (ext == False):
    userOptions(0)
    x = input(">>> ")
    if(x.lower() == "exit") :
        ext = True
    elif(x.lower() == "search"):
        searchMenu()
    else:
        print("This input is not recognised, sorry!")