function varargout = gui_v04(varargin)
% GUI_V04 M-file for gui_v04.fig
%      GUI_V04, by itself, creates a new GUI_V04 or raises the existing
%      singleton*.
%
%      H = GUI_V04 returns the handle to a new GUI_V04 or the handle to
%      the existing singleton*.
%
%      GUI_V04('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in GUI_V04.M with the given input arguments.
%
%      GUI_V04('Property','Value',...) creates a new GUI_V04 or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before gui_v04_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to gui_v04_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help gui_v04

% Last Modified by GUIDE v2.5 23-Sep-2017 19:51:06

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @gui_v04_OpeningFcn, ...
                   'gui_OutputFcn',  @gui_v04_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before gui_v04 is made visible.
function gui_v04_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to gui_v04 (see VARARGIN)

% Choose default command line output for gui_v04
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes gui_v04 wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = gui_v04_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
s = serial('COM10','BaudRate',9600)
out1 = instrfind('Port','COM10');
fclose(out1);
fopen(s)
% setappdata(0,'utama',gcf);
% setappdata(gcf,'nn',0);
h=1;
% n=[];
% m=[];
while h==1
%     utama=getappdata(0,'utama');
%     nn=getappdata(utama,'nn');
%     if nn==1
%         break
%     end
    ind=fscanf(s);
%     n=[str2num(ind);n];
%     %m=[m;str2num(ind)];
%     axes(handles.axes1);
%     %plot(m)
    set(handles.listbox1,'string',ind);
end

% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
setappdata(0,'utama',gcf);
setappdata(gcf,'nn',1);

% --- Executes on button press in pushbutton3.
function pushbutton3_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
delete(handles.figure1);

% --- Executes on selection change in listbox1.
function listbox1_Callback(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns listbox1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from listbox1


% --- Executes during object creation, after setting all properties.
function listbox1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
