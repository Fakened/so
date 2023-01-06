#!/bin/python3
# Importing dataclass module 
from dataclasses import dataclass 
  
# Annotation
@dataclass
  
# Class with attributes
class GeeksArticle(): 
    """A class for holding an article content"""
    
    # Attributes Declaration 
    # using Type Hints 
    topic: str
    contributor: str
    language: str
    upvotes: int
    
# A DataClass object 
article = GeeksArticle("DataClasses", "nightfury1", "Python", 1)
  
print(article)
with open("t", 'w') as f:
    f.write(str(article))