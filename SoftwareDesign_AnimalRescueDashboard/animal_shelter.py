import pymongo
import copy
from pymongo import MongoClient
from bson.objectid import ObjectId

class AnimalShelter:
    """CRUD operations for Animal collection in MongoDB"""
    
    def __init__(self):
        """Initialize the connection to the local MongoDB database and collection."""
        HOST = 'localhost'
        PORT = 27017
        DB = 'AAC'
        COL = 'animals'
        # Establish connection
        self.client = MongoClient(f'mongodb://{HOST}:{PORT}')
        self.database = self.client[DB]
        self.collection = self.database[COL]

    # ENHANCEMENT:  updated create method to validate inputs and catch duplicates
    def create(self, data):
        """Insert a document into the MongoDB collection with validation and duplicate key handling."""
        if data is not None and isinstance(data, dict): # Ensure input is a non-empty dictionary
            data = copy.deepcopy(data) # Create a copy to avoid mutating the original input

             # Define required fields expected in the document
            required_fields = {
                "name", "breed", "rescue_type",
                "age_upon_outcome_in_weeks", "location_lat", "location_long"
            }

             # Check that all required fields are present
            if not required_fields.issubset(data):
                print("Missing required fields.")
                return False

             # Attempt to convert string inputs to proper float types
            try:
                data["age_upon_outcome_in_weeks"] = float(data["age_upon_outcome_in_weeks"])
                data["location_lat"] = float(data["location_lat"])
                data["location_long"] = float(data["location_long"])
            except (ValueError, TypeError):
                print("Invalid data type for age or location fields.")
                return False
    
              # Remove _id if it exists to avoid MongoDB insert conflict
            if "_id" in data:
                print(f"Removing existing _id: {data['_id']}")
                del data["_id"]

              # Duplicate check
            if self.collection.find_one({"name": data["name"], "breed": data["breed"]}):
                print(f"Duplicate entry found for name: {data['name']} and breed: {data['breed']}")
                return False

            # Try inserting the document into the MongoDB collection
            try:
                print(f"Inserting document: {data}")
                result = self.collection.insert_one(data)
                return True if result.inserted_id else False # Return success based on insertion result
            except pymongo.errors.DuplicateKeyError as e: # Handle duplicate key violation
                print("Duplicate document detected. Entry not inserted.")
                return False
            except Exception as e: # Catch any other database errors
                print(f"Unexpected insertion error: {e}")
                return False
        else:
            # Reject invalid or improperly formatted input
            print("Invalid input. Must be a non-empty dictionary.")
            return False
    
    def read(self, query={}):
        """Find documents that match the given query in the collection."""
        try:
            cursor = self.collection.find(query)
            return list(cursor)  # Return as a list of documents
        except Exception as e:
            raise Exception(f"Error reading documents: {e}")
        
    def update(self, query, update_data):
        """Update documents in the collection that match the query."""
        if query and update_data:
            try:
                result = self.collection.update_many(query, update_data)
                return result.modified_count  # Number of documents modified
            except Exception as e:
                raise Exception(f"Error updating documents: {e}")
        raise ValueError("Query or update data is missing")

    def delete(self, query):
        """Delete documents in the collection that match the query."""
        if query:
            try:
                result = self.collection.delete_many(query)
                return result.deleted_count  # Number of documents deleted
            except Exception as e:
                raise Exception(f"Error deleting documents: {e}")
        raise ValueError("Query is missing")
