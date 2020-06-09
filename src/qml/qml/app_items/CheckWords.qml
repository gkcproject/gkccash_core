import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Rectangle{
    id:root
    color: "#212B50"

    property bool errVisible: false
    property bool check: true

    signal finishCheckWords()

    function init(){
    	check = true
    	mnemonicManager.RandomInit()
        errVisible = false
        row_0.init0()
        row_1.init1()
        row_2.init2()
        row_3.init3()
        row_4.init4()
    }

    function lockword()
    {
        row_1.lockword()
        row_2.lockword()
        row_3.lockword()
        row_4.lockword()
    }

    Component.onCompleted: {
        word1.selectedReceived.connect(selectWord)
        word2.selectedReceived.connect(selectWord)
        word3.selectedReceived.connect(selectWord)
        word4.selectedReceived.connect(selectWord)
        word5.selectedReceived.connect(selectWord)
        word6.selectedReceived.connect(selectWord)
        word7.selectedReceived.connect(selectWord)
        word8.selectedReceived.connect(selectWord)
        word9.selectedReceived.connect(selectWord)
        word10.selectedReceived.connect(selectWord)
        word11.selectedReceived.connect(selectWord)
        word12.selectedReceived.connect(selectWord)
        word13.selectedReceived.connect(selectWord)
        word14.selectedReceived.connect(selectWord)
        word15.selectedReceived.connect(selectWord)
        word16.selectedReceived.connect(selectWord)
        word17.selectedReceived.connect(selectWord)
        word18.selectedReceived.connect(selectWord)
        word19.selectedReceived.connect(selectWord)
        word20.selectedReceived.connect(selectWord)
        word21.selectedReceived.connect(selectWord)
        word22.selectedReceived.connect(selectWord)
        word23.selectedReceived.connect(selectWord)
        word24.selectedReceived.connect(selectWord)
    }

    function selectWord(word)
    {
        if(word_1.word_str==="")
        {
            word_1.word_str = word
        }else if(word_2.word_str==="")
        {
            word_2.word_str = word
        }else if(word_3.word_str==="")
        {
            word_3.word_str = word
        }else if(word_4.word_str==="")
        {
            word_4.word_str = word
            lockword()
        }
    }

    function checkWords()
    {
		if(!mnemonicManager.CheckMnemonicWord(word_1.index_str,word_1.word_str))
		{
			check = false
			errVisible = true
			word_1.word_color = "#EF5653"
		}
		if(!mnemonicManager.CheckMnemonicWord(word_2.index_str,word_2.word_str))
		{
			check = false
			errVisible = true
			word_2.word_color = "#EF5653"
		}
		if(!mnemonicManager.CheckMnemonicWord(word_3.index_str,word_3.word_str))
		{
			check = false
			errVisible = true
			word_3.word_color = "#EF5653"
		}
		if(!mnemonicManager.CheckMnemonicWord(word_4.index_str,word_4.word_str))
		{
			check = false
			errVisible = true
			word_4.word_color = "#EF5653"
		}
		if(check===true)
		{
			console.log("finishCheckWords")
    		mnemonicManager.SetMnemonicWords()			
			finishCheckWords()
		}
    }

    Label{
        id:label_1
        font.pixelSize:16
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.topMargin: 32
        anchors.leftMargin: 16
        color: "#FFFFFF"
        text:"Please re-enterthe mnemonic words to ensure that you havewritten it down correctly."
    }

    Row{
        id:row_0
        anchors.top:label_1.bottom
        anchors.left: label_1.left
        anchors.topMargin: 32
        height: 40
        spacing:24

        function init0()
        {
            word_1.index_str = mnemonicManager.GetRandomIndex(1)
            word_2.index_str = mnemonicManager.GetRandomIndex(2)
            word_3.index_str = mnemonicManager.GetRandomIndex(3)
            word_4.index_str = mnemonicManager.GetRandomIndex(4)
            word_1.word_str = ""
            word_2.word_str = ""
            word_3.word_str = ""
            word_4.word_str = ""
            word_1.word_color = "#FFFFFF"
            word_2.word_color = "#FFFFFF"
            word_3.word_color = "#FFFFFF"
            word_4.word_color = "#FFFFFF"
        }

        Seedword{
            id:word_1
            index_str: "2"
            word_str: ""
        }

        Seedword{
            id:word_2
            index_str: "7"
            word_str: ""
        }

        Seedword{
            id:word_3
            index_str: "15"
            word_str: ""
        }

        Seedword{
            id:word_4
            index_str: "21"
            word_str: ""
        }
    }

    Label{
        id:label_2
        font.pixelSize:16
        anchors.top:row_0.bottom
        anchors.left: row_0.left
        anchors.topMargin: 40
        color: "#FFFFFF"
        text:"Please click the mnemonic according to the above serial number"
    }

    Row{
        id:row_1
        anchors.top:label_2.bottom
        anchors.left: label_2.left
        anchors.topMargin: 24
        height: 35
        spacing:8

        function init1()
        {
            word1.init()
            word2.init()
            word3.init()
            word4.init()
            word5.init()
            word6.init()
            word1.word_str = mnemonicManager.GetRandomMnemonicWord(1)
            word2.word_str = mnemonicManager.GetRandomMnemonicWord(2)
            word3.word_str = mnemonicManager.GetRandomMnemonicWord(3)
            word4.word_str = mnemonicManager.GetRandomMnemonicWord(4)
            word5.word_str = mnemonicManager.GetRandomMnemonicWord(5)
            word6.word_str = mnemonicManager.GetRandomMnemonicWord(6)
        }

        function lockword()
        {
			word1.selected = false
			word2.selected = false
			word3.selected = false
			word4.selected = false
			word5.selected = false
			word6.selected = false
        }

        Seedword{
            id:word1
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word2
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word3
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word4
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word5
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word6
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }
    }

    Row{
        id:row_2
        anchors.top:row_1.bottom
        anchors.left: row_1.left
        anchors.topMargin: 16
        height: 35
        spacing:8

        function init2()
        {
            word7.init()
            word8.init()
            word9.init()
            word10.init()
            word11.init()
            word12.init()
            word7.word_str = mnemonicManager.GetRandomMnemonicWord(7)
            word8.word_str = mnemonicManager.GetRandomMnemonicWord(8)
            word9.word_str = mnemonicManager.GetRandomMnemonicWord(9)
            word10.word_str = mnemonicManager.GetRandomMnemonicWord(10)
            word11.word_str = mnemonicManager.GetRandomMnemonicWord(11)
            word12.word_str = mnemonicManager.GetRandomMnemonicWord(12)
        }

        function lockword()
        {
			word7.selected = false
			word8.selected = false
			word9.selected = false
			word10.selected = false
			word11.selected = false
			word12.selected = false
        }

        Seedword{
            id:word7
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word8
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word9
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word10
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word11
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word12
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }
    }

    Row{
        id:row_3
        anchors.top:row_2.bottom
        anchors.left: row_2.left
        anchors.topMargin: 16
        height: 35
        spacing:8

        function init3()
        {
            word13.init()
            word14.init()
            word15.init()
            word16.init()
            word17.init()
            word18.init()
            word13.word_str = mnemonicManager.GetRandomMnemonicWord(13)
            word14.word_str = mnemonicManager.GetRandomMnemonicWord(14)
            word15.word_str = mnemonicManager.GetRandomMnemonicWord(15)
            word16.word_str = mnemonicManager.GetRandomMnemonicWord(16)
            word17.word_str = mnemonicManager.GetRandomMnemonicWord(17)
            word18.word_str = mnemonicManager.GetRandomMnemonicWord(18)
        }

        function lockword()
        {
			word13.selected = false
			word14.selected = false
			word15.selected = false
			word16.selected = false
			word17.selected = false
			word18.selected = false
        }

        Seedword{
            id:word13
            width: 75
            height: 35
            indexVisible: false
        }

        Seedword{
            id:word14
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word15
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word16
            width: 75
            height: 35
            indexVisible: false
        }

        Seedword{
            id:word17
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word18
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }
    }

    Row{
        id:row_4
        anchors.top:row_3.bottom
        anchors.left: row_3.left
        anchors.topMargin: 16
        height: 38
        spacing:8

        function init4()
        {
            word19.init()
            word20.init()
            word21.init()
            word22.init()
            word23.init()
            word24.init()
            word19.word_str = mnemonicManager.GetRandomMnemonicWord(19)
            word20.word_str = mnemonicManager.GetRandomMnemonicWord(20)
            word21.word_str = mnemonicManager.GetRandomMnemonicWord(21)
            word22.word_str = mnemonicManager.GetRandomMnemonicWord(22)
            word23.word_str = mnemonicManager.GetRandomMnemonicWord(23)
            word24.word_str = mnemonicManager.GetRandomMnemonicWord(24)
        }

        function lockword()
        {
			word19.selected = false
			word20.selected = false
			word21.selected = false
			word22.selected = false
			word23.selected = false
			word24.selected = false
        }

        Seedword{
            id:word19
            width: 75
            height: 35
            indexVisible: false
            selected: true
            word_str: "aaa"
        }

        Seedword{
            id:word20
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word21
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word22
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word23
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }

        Seedword{
            id:word24
            width: 75
            height: 35
            indexVisible: false
            selected: true
        }
    }

    Label{
        id:error_label
        font.pixelSize:16
        anchors.top:row_4.bottom
        anchors.left: row_4.left
        anchors.topMargin: 24
        color: "#EF5653"
        visible:errVisible
        text:"Your entered words do not match, please press back to re-check your mnemonic."
    }

}
