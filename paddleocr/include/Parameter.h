// Copyright (c) 2021 raoyutian Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#pragma pack(push,1)
#include <vector>
using namespace std;
struct OCRParameter
{
	//通用参数
	bool use_gpu;
	int gpu_id;
	int gpu_mem;
	int numThread;
	bool enable_mkldnn;
	
	//检测模型相关
	int    Padding;
	int    MaxSideLen;
	float  BoxScoreThresh;
	float   BoxThresh;
	float   UnClipRatio;
	bool use_polygon_score;
	bool visualize;
	bool    DoAngle;
	bool   MostAngle;
	
	//方向分类器相关
	bool use_angle_cls;
	float   cls_thresh;

	OCRParameter()
	{
		//通用参数
		use_gpu = false;
		gpu_id = 0;
		gpu_mem = 4000;
		numThread = 4;
		enable_mkldnn = true;
		
		//检测模型相关
		Padding = 50;
		MaxSideLen = 2048;
		BoxThresh = 0.3f;
		BoxScoreThresh = 0.5f;
		UnClipRatio = 1.8f;
		use_polygon_score = true;
		visualize = true;

		DoAngle = true;
		MostAngle = true;

		//方向分类器相关
		use_angle_cls = false;
		cls_thresh = 0.9f;
	}
};

struct Textblock {
	
	std::wstring textblock;
	std::vector<std::vector<int>> box;
	float score;
	Textblock(wstring textblock, std::vector<std::vector<int>> box,float score) {
		this->textblock = textblock;
		this->box = box;
		this->score = score;
	}
};

//textblock文本四个角的点
struct _OCRTextPoint {
	int x;
	int y;
	_OCRTextPoint() :x(0), y(0) {
	}
};

struct _OCRText {
	//textblock文本
	int textLen;
	char* ptext;
	//一个textblock四个点
	_OCRTextPoint points[4];
	//得分
	float score;
	_OCRText() {
		textLen = 0;
		ptext = nullptr;
		score = 0.0f;
	}
};

typedef struct _OCRResult {
	//textblock文本个数
	int textCount;
	_OCRText* pOCRText;
}OCRResult, * LpOCRResult;
#pragma pack(pop) 
