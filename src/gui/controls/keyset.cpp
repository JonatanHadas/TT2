#include "keyset.h"

#include "../../utils/serialization.h"
#include "../../utils/utils.h"

#include <fstream>

template<>
class Serializer<SDL_Scancode>{
public:
	static void serialize(ostream& output, SDL_Scancode value) { serialize_int<int, 4>(output, value); }
	static SDL_Scancode deserialize(istream& input) { return (SDL_Scancode)deserialize_int<int, 4>(input); }
};

void KeySet::serialize(ostream& output) const{
	serialize_value(output, left);
	serialize_value(output, right);
	serialize_value(output, forward);
	serialize_value(output, back);
	serialize_value(output, shoot);
}

KeySet KeySet::deserialize(istream& input){
	KeySet value;
	
	value.left = deserialize_value<SDL_Scancode>(input);
	value.right = deserialize_value<SDL_Scancode>(input);
	value.forward = deserialize_value<SDL_Scancode>(input);
	value.back = deserialize_value<SDL_Scancode>(input);
	value.shoot = deserialize_value<SDL_Scancode>(input);
	
	return value;
}

KeyController::KeyController(const KeySet& key_set) : key_set(key_set) {}

void KeyController::handle_event(const SDL_Event& event){
	if(event.type == SDL_KEYDOWN){
		if(event.key.keysym.scancode == key_set.left) state.left = true;
		if(event.key.keysym.scancode == key_set.right) state.right = true;
		if(event.key.keysym.scancode == key_set.forward) state.forward = true;
		if(event.key.keysym.scancode == key_set.back) state.back = true;
		if(event.key.keysym.scancode == key_set.shoot) state.shoot = true;
	}
}
KeyState KeyController::get_state(){
	KeyState result = state;
	
	auto keyboard_state = SDL_GetKeyboardState(NULL);
	state.left = keyboard_state[key_set.left];
	state.right = keyboard_state[key_set.right];
	state.forward = keyboard_state[key_set.forward];
	state.back = keyboard_state[key_set.back];
	state.shoot = keyboard_state[key_set.shoot];
	
	return result;
}

KeySetManager::KeySetManager(const char* filename) :
	filename(filename) {
		
	load();
}

void KeySetManager::load(){
	ifstream file;
	file.open(filename, ios::in | ios::binary);
	if(!file.is_open()) return;

	keysets = deserialize_value<vector<KeySet>>(file);
	
	file.close();
}

void KeySetManager::save() const {
	ofstream file;
	file.open(filename, ios::out | ios::binary);
	
	serialize_value<vector<KeySet>>(file, keysets);
	
	file << fflush;
	
	file.close();
}

int KeySetManager::get_index(int id) const {
	for(int i = 0; i < ids.size(); i++){
		if(ids[i] == id) return i;
	}
	return -1;
}

const KeySet& KeySetManager::get_keys(int id) const {
	return keysets[get_index(id)];
}

void KeySetManager::set_keys(int id, const KeySet& keys){
	keysets[get_index(id)] = keys;
	
	save();
}

int KeySetManager::get_new(){
	ids.push_back(next_id++);
	if(ids.size() > keysets.size()){
		keysets.push_back(KeySet());
	}
	
	return ids.back();
}

void KeySetManager::remove(int id){
	int index = get_index(id);
	
	KeySet keys = pop_index(keysets, index);
	remove_index(ids, index);
	
	keysets.insert(keysets.begin() + ids.size(), keys);
	
	save();
}
