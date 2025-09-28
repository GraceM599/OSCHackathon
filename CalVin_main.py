import json
from difflib import get_close_matches
import requests



name=""

# Load CalVin knowledge from JSON
def load_knowledge(file_path: str) -> dict:
    with open(file_path,'r') as file:
        data: dict=json.load(file)
    return data

def save_knowledge(file_path: str, data: dict):
    with open(file_path,'w') as file:
        json.dump(data,file,indent=2)

def find_match(user_question: str, questions: list[str]) -> str | None:
    matches: list = get_close_matches(user_question,questions,n=1,cutoff=0.9)
    return matches[0] if matches else None

def get_answer(question: str, knowledge_base: dict) -> str | None:
    for a in knowledge_base["questions"]:
        if a["question"]==question:
            return a["answer"]

def check_weather(city,api_key):
    url = "http://api.openweathermap.org/data/2.5/weather"
    params = {
        "q":city,
        "appid":api_key,
        "units":"imperial"
    }

    response = requests.get(url, params=params)

    if response.status_code==200:
        data=response.json()
        weather_description=data['weather'][0]['description']
        temp=data['main']['temp']
        feels_like=data['main']['feels_like']
        humidity=data['main']['humidity']

        forecast= (f"CalVin: Here's the weather for today!:\n"
                    f"        Description: {weather_description}\n"
                    f"        Temperature: {temp:.0f}°F\n"
                    f"        Feels Like: {feels_like:.0f}°F\n"
                    f"        Humidity: {humidity}%")
        return forecast
    else:
        return f"CalVin: ERR0R! Could not check the weather for {city}. (Error Code: {response.status_code})"


def calvin():
    knowledge_base: dict = load_knowledge('knowledge.json')

    while True:
        with open('input.txt','r') as input_file:
            user_input=input_file.read()

        if user_input.lower()=='quit' or user_input.lower()=='bye' or user_input.lower()=='goodbye':
            answer="Goodbye now!"
            full_outputs = []
            full_output = {
                "User": user_input,
                "CalVin": answer
            }
            full_outputs.append(full_output)
            with open('output.json', 'w') as output_file:
                json.dump(full_outputs, output_file)
            with open('output.json', 'r') as output_file:
                read_output = json.load(output_file)
            for entry in read_output:
                print(f"User: {entry['User']}")
                print(f"CalVin: {entry['CalVin']}")
            break
        elif user_input.lower()=='pull up the weather' or user_input.lower()=='check the weather' or user_input.lower()=='whats the weather?' or user_input.lower()=='hows the weather?':
            print(check_weather("Gainesville","f42818897729e12f9a47791c21dbb84f"))

        match: str | None =find_match(user_input, [a["question"] for a in knowledge_base["questions"]])

        if match:
            answer: str=get_answer(match,knowledge_base)
        else:
            answer="CalVin: I don't know how to answer that one yet! But I hope I will soon"
            new_answer: str=input('Type what CalVin should say or enter "skip" to move on: ')

            if new_answer.lower()!="skip":
                knowledge_base["questions"].append({"question":user_input,"answer":new_answer})
                save_knowledge("knowledge.json",knowledge_base)
                print("CalVin: Thank you kindly! I'll be sure to remember that ^^")
        full_outputs=[]
        full_output = {
            "User": user_input,
            "CalVin": answer
        }
        full_outputs.append(full_output)
        with open('output.json', 'w') as output_file:
            json.dump(full_outputs, output_file)
        with open('output.json', 'r') as output_file:
            read_output = json.load(output_file)
        for entry in read_output:
            print(f"User: {entry['User']}")
            print(f"CalVin: {entry['CalVin']}")
        break


if __name__=="__main__":
    calvin()