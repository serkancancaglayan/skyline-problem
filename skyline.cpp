//Serkan Can Caglayan 18120205021
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <set>
#include <algorithm>
#include <stdlib.h>
#include <bits/stdc++.h>

using namespace std;

//gonderilen stringi, gonderilen karaktere gore ayirir
vector<string> split(string strToSplit, char delimeter){
    stringstream ss(strToSplit);
    string item;
	vector<string> splittedStrings;
    while (getline(ss, item, delimeter))
	{
		splittedStrings.push_back(item);
    }
	return splittedStrings;
}

//csvden verileri okuyarak noktalari tutan vektor olusturur
vector<vector<int>> read_from_csv(const char * filename){
    vector<vector<int>> rects;
    ifstream fin;
    string line;
    fin.open(filename);
    while(!fin.eof()){
        fin >> line;
        vector<string> v = split(line, ',');
        vector<int> rect;
        for(int i = 0; i < v.size(); i++){
            rect.push_back(stoi(v[i]));
        }
        //genisligi bitis koordinati olarak degistiriyorum
        rect[0] = rect[0] + rect[2];

        rects.push_back(rect);
    }
    fin.close();
    return rects;
}


// T(n) = 2 * T(n / 2) + n --> T(n) = O(nlogn)
vector<vector<int>> divide_and_conquer(vector<vector<int>> rects, int begin, int end){
    vector<vector<int>> final_rect;
    if(begin > end){
        return final_rect;
    }
    else if(begin == end){
        vector<int> v1(rects[begin][2], rects[begin][1]);
        vector<int> v2(rects[begin][0], 0);
        final_rect.push_back(v1);  
        final_rect.push_back(v2);
        return final_rect;
    }

    int mid = (begin + end) / 2;
    vector<vector<int>> left = divide_and_conquer(rects, begin, mid);
    vector<vector<int>> right = divide_and_conquer(rects, mid + 1, end);
    int left_r = 0, right_r = 0;
    //O(n)
    while(!left.empty() || !right.empty()){
        int x1 = left[0][2];
        int x2 = right[0][2];
        int x = 0;
        if(x1 < x2){
            //pop
            left_r = left[0][0];
            left.erase(left.begin(), left.begin() + 1);
        }
        else if(x1 > x2){
            //pop
            right_r = right[0][0];
            right.erase(right.begin(), right.end());
        }
        else{ // x1 == x2
            left_r = left[0][0];
            left.erase(left.begin(), left.begin() + 1);
            right_r = right[0][0];
            right.erase(right.begin(), right.end());
        }

        //current max heighti buluyoruz
        int height = left_r > right_r ? left_r : right_r;
        if(final_rect.empty() || height != final_rect.back()[0]){
            vector<int> temp{x, height};
            final_rect.push_back(temp);
        }
    }
    return final_rect;
}

//T(n) = O(n) + O(nlogn) + O(n) --> T(n) = O(nlogn)
vector<vector<int>> dynamic(vector<vector<int>> rects) {
    vector<vector<int>> final_rect;
    vector<pair<int, int>> points;
    //n * 2 * O(1) = O(n)
    for (auto & b : rects) {
        points.push_back(pair<int, int>(b[2], -b[1])); //O(1) eger kapasite asilmazsa
        points.push_back(pair<int, int>(b[0], b[1]));  //O(1) eger kapasite asilmazsa
    }
    //O(nlogn)
    sort(points.begin(), points.end());
    multiset<int> m{0};
    int pre = 0, cur = 0;
    //n * O(1) = O(n)
    for (auto & p : points) {
        if (p.second < 0) {
            //O(1)
            m.insert(-p.second);
        } else {    
            //O(1)
            m.erase(m.find(p.second));
        }
        cur = *m.rbegin();
        if (cur != pre) {
            //O(1) eger kapasite asilmazsa   
            final_rect.push_back({p.first, cur});
            pre = cur;
        }
    }
    return final_rect;
}

//T(n) = n * 2 * O(n) --> T(n) = O(n^2)
vector<int> brute_force(vector<vector<int>> rects){
    int line_sweep_array[1000] = {0};
    int min = INT_MAX;
    int max = INT_MIN;
    vector<vector<int>>final_rect;
    //O(n)
    for(vector<int> &v : rects){
        vector<int> temp;
        int right = v[0] , height = v[1], left = v[2];
        min = left < min ? left : min;
        max = right > max ? right : max;
        //O(n) left - right arasi  max yuksekligi buluyoruz
        for(int i = left; i < right; i++){
            if(line_sweep_array[i] < height){
                line_sweep_array[i] = height;
            }   
        }
        int x = line_sweep_array[min];
        temp.push_back(min);
        temp.push_back(x);
        //O(n)
        for(int i = min + 1; i <= max; i++){
            if(x != line_sweep_array[i]){
                x = line_sweep_array[i];
                temp.push_back(i);
                temp.push_back(x);
            }
        }
        final_rect.push_back(temp);
    }
    return final_rect.back();
}
//olusan tum dikdortgenlerin uzerini kaplayacak ipin uzunlugunu hesaplar
int calculate_string_len(vector<vector<int>> final_rect){
    int string = final_rect[0][1];
    for(int i = 0; i < final_rect.size() - 1; i++){
        string += abs(final_rect[i][0] - final_rect[i + 1][0]);
        string += abs(final_rect[i][1] - final_rect[i + 1][1]);
    }
    return string;
}

//olusan tum dikdortgenlerin uzerini kaplayacak ipin uzunlugunu hesaplar
int calculate_string_len(vector<int> final_rect){
    int string = final_rect[1];
    for(int i = 0; i < final_rect.size() - 2; i++){
         string += abs(final_rect[i] - final_rect[i + 2]);
    }
    return string;
}

int main(){
    string file_name;
    cout << "CSV FILE NAME : "; cin >> file_name;
    vector<vector<int>> rects = read_from_csv(file_name.c_str());

    vector<vector<int>> res_dynamic = dynamic(rects);
    vector<int> res_brute = brute_force(rects);
    //vector<vector<int>> res_divide = divide_and_conquer(rects, 0 , rects.size() - 1);
    
    cout << "Dynamic: " << calculate_string_len(res_dynamic) << endl;
    cout << "Brute Force: " << calculate_string_len(res_brute) << endl;
    //cout << "Divide and Conquer: " << calculate_string_len(res_divide) << endl;
}