Smart Reservations
===================

Smart Reservations is a Drupal module consisting of a reservation system of holiday apartments, hotel rooms, villas or any type of accommodation.

Its main goal is to provide a simple way of managing and maintaining the rates and fees per person and night.

Originally created for Casafont rural holiday apartments by [ray17n](http://www.ramon.click/), and now publicly available and licensed with a GNU General Public License.

More info at www.ramon.click/smart-reservations.


Rates
-----

### Calculation

- When there is no Custom rate / fee for a specific date, the system uses the Default rates / fees to calculate the rates per night.
- When there is no Default rate / fee for a specific night of the week (Sun, Mon, Tue, Wed, Thu, Fri, Sat), the system uses the General default rates / fees to calculate the rates per night.
- The system applies a final Rate multiplier for each specific accommodation unit.

### Parameters

- **Base rate**. The base rate per night and person.
Children discount. Discount applied for each child per night. Example. with a Base rate of 25, and a Children discount of 5, the rate for each child will be 20.
- **Dogs fee**. Fee applied for dog per night.
- **Single night fee**. Additional fee for single night reservations.
- **Low occupancy fee**. Additional fee per person for low occupancy reservations.
- **Low occupancy threshold**. Number of guests below which (<=) the system applies the low ocuppancy fee. For example, if we have a threshold of 2 guests, and a low occcupancy fee of 10, the system will apply a low occupancy fee of 20, but if we have 3 guests there will be no low occupancy fee.



Data model
----------

### Entites

- **Accommodation unit**. Any kind of hotel room, holiday apartment, villa, bungalow, lodge, etc. The instances of this entity are stored with a Drupal content type.
- **Reservation**. A reservation of one or more Accommodation units for one or more nights. The instances of this entity are stored with a Drupal content type.
- **Reservation items**. Reservation items per date for a specific reservation. The instances of this entity are stored in a database table.
- **Custom rate per night**. Custom rates per date create by the business administrator. The instances of this entity are stored in a database table.
- **Search log**. Reservation search queries submitted. The instances of this entity are stored in a database table.

### Schema

![smart reservations model](http://www.ramon.click/sites/default/files/smart_reservations/smart-reservations-entity-relationship.png "smart reservations model")


Todos
----------------------------

### Ideas or new features to add:
  - Accomodation units. Show an image in search results form.
  - Ubercart support.
  - Support for other pets and options.
  - Limit submission searches per hour.


### To improve:
  - Reservations:
    - create an automatic alias reservations/*,
  - Accommodation units:
    - create an automatic alias accommodation_units/*,
    - show a calculated rate sample applying the multiplier rate.
  - Database schema:
    - check unique indexes/keys for tables
  - Rates per night. Improve entry data workflow.


Change log
--------------------

### v7.x-1.0
Date: 2015-03-02

- Public available / GNU licence.
- Simplified workflow for making a new reservation.
- Form settings with vertical tabs.
- Default rates for
- AJAX support in Search available form. Add/Remove accomodation unit. Improved functionalities.


###v7.x-0.2
Date: 2014-11-15

- Drupal 7 version.
- Custom rates per night.


###v6.x-0.2
Date: 2012-06-14

- First version Drupal 6.
- Custom module developed for Casafont Rural Holiday Apartments.



Screenshots
-------------------

#### Reservations
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-make-reservation.png "smart reservations screenshot")

#### Default rates
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-default-rates-2.png "smart reservations screenshot")
#### Rates per night
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-rates-per-night.png "smart reservations screenshot")
#### General settings
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-settings-1.png "smart reservations screenshot")
#### Currency format settings
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-settings-2.png "smart reservations screenshot")
#### Search parameters settings
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-settings-3.png "smart reservations screenshot")
#### Messages settings
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-settings-4.png "smart reservations screenshot")
