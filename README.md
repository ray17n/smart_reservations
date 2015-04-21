# Smart Reservations

Smart Reservations is a Drupal module consisting of a reservation system of holiday apartments, hotel rooms, villas or any type of accommodation.

Its main goal is to provide a simple way of managing and maintaining the rates and fees per person and night.

Originally created for Casafont rural holiday apartments by [ray17n](http://www.ramon.click/), and now publicly available and licensed with a GNU General Public License.

More info at www.ramon.click/smart-reservations.

## Rates

### Calculation

- When there is no **Custom rate / fee** for a specific date, the system uses the **Default rates / fees** to calculate the rates per night.
- When there is no **Default rate / fee for a specific night** of the week (Sun, Mon, Tue, Wed, Thu, Fri, Sat), the system uses the **General default rates / fees** to calculate the rates per night.
- The system applies a final **Rate multiplier** for each specific accommodation unit.

### Parameters

- **Base rate**. The base rate per night and person.
- **Children discount**. Discount applied for each child per night. Example. with a Base rate of 25, and a Children discount of 5, the rate for each child will be 20.
- **Dogs fee**. Fee applied for dog per night.
- **Single night fee**. Additional fee for single night reservations.
- **Low occupancy fee**. Additional fee per person for low occupancy reservations.
- **Low occupancy threshold**. Number of guests below which (<=) the system applies the low ocuppancy fee. For example, if we have a threshold of 2 guests, and a low occcupancy fee of 10, the system will apply a low occupancy fee of 20, but if we have 3 guests there will be no low occupancy fee.


## Data model

### Entities

- **Accommodation unit**. Any kind of hotel room, holiday apartment, villa, bungalow, lodge, etc. The instances of this entity are stored with a Drupal content type.
- **Reservation**. A reservation of one or more Accommodation units for one or more nights. The instances of this entity are stored with a Drupal content type.
- **Reservation items**. Reservation items per date for a specific reservation. The instances of this entity are stored in a database table.
- **Custom rate per night**. Custom rates per date create by the business administrator. The instances of this entity are stored in a database table.
- **Search log**. Reservation search queries submitted. The instances of this entity are stored in a database table.

### Schema

![smart reservations model](http://www.ramon.click/sites/default/files/smart_reservations/smart-reservations-entity-relationship.png "smart reservations model")


### Reservation status
*(currently in development)*

![smart reservations model](http://www.ramon.click/sites/default/files/smart_reservations/smart-reservations-reservation-states-0.png "smart reservations states")


##Installation

- Option 1:
  - git clone https://github.com/ray17n/smart_reservations.git into your_drupal_website/sites/all/modules
- Option 2:
  - Download the module from https://github.com/ray17n/smart_reservations/archive/master.zip and uncompress the file into your_drupal_website/sites/all/modules

  You can also install it into your_drupal_website/sites/all/modules/contrib.

- Enable the module.
- Configure the general settings and rates:
  - **Settings**: your_drupal/admin/config/content/smart-reservations
  - **Default rates**: your_drupal/admin/config/content/smart-reservations/default-rates
  - **Custom rates** (and calculated default values): your_drupal/admin/config/content/smart-reservations/rates
  - **Search log**: your_drupal/admin/config/content/smart-reservations/log

- Create the accommodation units you need: Content - Add content - Accommodation Unit. The module also creates two sample ones, you can edit these ones or delete them.

- The module also creates a block for searching reservations. The name of the block is **Search reservations**. You can also find it at your_drupal/admin/structure/block/manage/smart_reservations/search-reservations/configure

- After submitting a search the block redirects to the page **your_drupal/search-available/search_parms...** You can change the URL of this page in the settings form.

- You can also use the page **your_drupal/search-available** without using the block, and add a menu entry of this url to your main menu. You can also change the URL of this page in the settings form.

- If an Accommodation unit is unpublished will not appear at the search availability results.


##To dos

### Currently working
  - Reservations:
    - After submitting, add more info into the message to the administrator.
    - After submitting, add more info into the message to the client.
    - Improve workflow when creating a new reservation with an admin account.
    - Improve editing functionality.
    - Improve status workflow.
  - Rates per night. Improve entry data workflow.
  - When making a reservation, control if user goes back with the back button of the web browser.

### Ideas / new features to add
  - Limit submission searches per hour. Analyze if honeypot module can be useful.
  - Accomodation units. Show an image in search results form.
  - Ubercart support.
  - Support for other pets and options.
  - Calendar in search results: change range with a click.
  - Add Discounts per date.
  - Reports of searched reservations per day.
  - Limit the number of search queries per minute / create a parameter.
  - Automatically localize the first day of the week when showing results.
  - When listing rates, add a calendar view.
  - Change module structure folder to allow having other modules in the same main folder.

## Change log

### v7.x-1.1
  - Reservations:
    - Total amount aligned to the right.
    - New status field: Created, Submitted, Pending customer verification, Pending confirmation payments, Confirmed, Checked-in, Completed, Cancelled.
  - Accommodation units:
    - Show a calculated rate sample applying the multiplier rate in the accommodation unit page.
    - Added a new tab with information of the reservations.
    - There's now a sample of the calculated multiplier rate in the description of the multipglier rate field in the accomodation unit page and edit page.
  - Initial options of Accommodation Unit and Reservation content types  are now controlled programatically.
  - Buttons are now color coded:
    - Green: Make reservation, Add to the reservation, Submit
    - Red: Remove
  - Schema. Revision of primary keys and indexes.
  - The URL of the Search available results page is now a parameter. Default value is 'search-available'.
  - Misc.
    - Added smart_reservations_preprocess_node(&$variable)
    - Auxiliary function sr_curr($value) is now located in smart_reservations.module file.
    - Custom templates functionality added (smart_reservations_theme_registry_alter method).
    - Added custom template for accommodation units (node--sr_accommodation_unit.tpl.php).


### v7.x-1.0
Date: 2015-03-02

- Public available with GNU licence.
- Simplified workflow for making a new reservation.
- Form settings with vertical tabs.
- Default rates and Custom rates combined list.
- AJAX support in Search available.
- AJAX support in Add/Remove accomodation unit buttons.


###v7.x-0.2
Date: 2014-11-15

- Drupal 7 version.
- Custom rates per night.


###v6.x-0.2
Date: 2012-06-14

- First version Drupal 6.
- Custom module developed for Casafont Rural Holiday Apartments.



## Screenshots
-------------------

#### Search availability results
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-availability-results-b3.png "smart reservations screenshot")

#### Search availability block
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-search-block.png "smart reservations screenshot")

#### Submit a reservation
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-make-reservation-b1.png "smart reservations screenshot")


#### Default rates
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-default-rates-2.png "smart reservations screenshot")
#### Rates per night
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-rates-per-night.png "smart reservations screenshot")
#### General settings
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-settings-1.png "smart reservations screenshot")
#### Currency format settings
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-settings-2.png "smart reservations screenshot")
#### Search parameters settings
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-settings-3-new.png "smart reservations screenshot")
#### Messages settings
![screenshot](http://www.ramon.click/sites/default/files/smart_reservations/screenshots/smart-reservations-settings-4.png "smart reservations screenshot")
