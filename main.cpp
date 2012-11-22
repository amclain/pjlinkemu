/* 
 * PJLink Projector Emulator
 * 
 * File:   main.cpp
 * Author: Alex McLain <alex@alexmclain.com>
 * 
 * Website:
 * [TBD]
 * 
 * PJLink Specification:
 * http://pjlink.jbmia.or.jp/english/data/PJLink%20Specifications100.pdf
 * 
 * 
 * ========================================================================
 * Copyright 2012 Alex McLain
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "UI.h"

int main(int argc, char** argv) {
    UI *ui = UI::getInstance();
    ui->initialize();
    ui->shutdown();
    return 0;
}

