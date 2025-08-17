from pymongo import MongoClient
from bson.objectid import ObjectId

class AnimalShelter:
    """CRUD operations for Animal collection in MongoDB"""
    
    def __init__(self):
        """Initialize the connection to the MongoDB database and collection."""
        USER = 'aacuser'
        PASS = 'SNHU1234'
        HOST = 'nv-desktop-services.apporto.com'
        PORT = 33117
        DB = 'AAC'
        COL = 'animals'
        # Establish connection
        self.client = MongoClient(f'mongodb://{USER}:{PASS}@{HOST}:{PORT}')
        self.database = self.client[DB]
        self.collection = self.database[COL]

    def create(self, data):
        """Insert a document into the MongoDB collection."""
        if data:
            try:
                result = self.collection.insert_one(data)
                return True if result.inserted_id else False
            except Exception as e:
                raise Exception(f"Error inserting document: {e}")
        else:
            raise ValueError("Nothing to save, the data is empty")
    
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
