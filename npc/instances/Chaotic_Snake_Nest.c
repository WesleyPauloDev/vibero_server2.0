//===== rAthena Script =======================================
//= Chaotic/Confused Snake's Nest
//= Converted from Episode 19 script collection for this server.
//============================================================

-	script	ChaoticSnakeNest_Instance#Custom	-1,{
	if (!attachrid(#INSTANCERID)) end;
	#INSTANCERID = 0;

OnStartInstance:
	callfunc("F_InstanceEntry",
		"Confused Snake's Nest",
		"Erudito Arcano",
		"confused_snake_nest",
		"ConfusedSnakeNest",
		200,
		"Ninho da Serpente Confusa"
	);
	end;
}
-	script	dummy_csn	-1,{ end; }

1@jorchs,1,1,1	script	#191_csn_control	-1,{
	end;
	
OnInstanceInit:
	'map$ =instance_mapname("1@jorchs");
	'csn =0;
	'summon =0;
	'elite =0;
	'clear =0;
	'dev_1 =0;
	'dev_2 =0;
	'dev_3 =0;
	'dev_4 =0;
	'dev_5 =0;
	'csn_done =0;
	setd("'19m00",0);
	setd("'19m01",0);
	setd("'19m10",0);
	setd("'19m11",0);
	
	//= First Wall
	setcell 'map$,139,59,139,59,CELL_WALKABLE,false;
	setcell 'map$,138,58,138,58,CELL_WALKABLE,false;
	setcell 'map$,137,57,137,57,CELL_WALKABLE,false;
	setcell 'map$,139,59,139,59,CELL_SHOOTABLE,false;
	setcell 'map$,138,58,138,58,CELL_SHOOTABLE,false;
	setcell 'map$,137,57,137,57,CELL_SHOOTABLE,false;
	//= Second Wall
	setcell 'map$,120,129,120,129,CELL_WALKABLE,false;
	setcell 'map$,121,128,121,128,CELL_WALKABLE,false;
	setcell 'map$,122,127,122,127,CELL_WALKABLE,false;
	setcell 'map$,123,127,123,127,CELL_WALKABLE,false;
	setcell 'map$,124,126,124,126,CELL_WALKABLE,false;
	setcell 'map$,125,125,125,125,CELL_WALKABLE,false;
	setcell 'map$,126,125,126,125,CELL_WALKABLE,false;
	setcell 'map$,120,129,120,129,CELL_SHOOTABLE,false;
	setcell 'map$,121,128,121,128,CELL_SHOOTABLE,false;
	setcell 'map$,122,127,122,127,CELL_SHOOTABLE,false;
	setcell 'map$,123,127,123,127,CELL_SHOOTABLE,false;
	setcell 'map$,124,126,124,126,CELL_SHOOTABLE,false;
	setcell 'map$,125,125,125,125,CELL_SHOOTABLE,false;
	setcell 'map$,126,125,126,125,CELL_SHOOTABLE,false;
	//= Third Wall
	setcell 'map$,249,177,249,177,CELL_WALKABLE,false;
	setcell 'map$,248,176,248,176,CELL_WALKABLE,false;
	setcell 'map$,247,176,247,176,CELL_WALKABLE,false;
	setcell 'map$,246,176,246,176,CELL_WALKABLE,false;
	setcell 'map$,245,176,245,176,CELL_WALKABLE,false;
	setcell 'map$,244,176,244,176,CELL_WALKABLE,false;
	setcell 'map$,243,175,243,175,CELL_WALKABLE,false;
	setcell 'map$,242,174,242,174,CELL_WALKABLE,false;
	setcell 'map$,249,177,249,177,CELL_SHOOTABLE,false;
	setcell 'map$,248,176,248,176,CELL_SHOOTABLE,false;
	setcell 'map$,247,176,247,176,CELL_SHOOTABLE,false;
	setcell 'map$,246,176,246,176,CELL_SHOOTABLE,false;
	setcell 'map$,245,176,245,176,CELL_SHOOTABLE,false;
	setcell 'map$,244,176,244,176,CELL_SHOOTABLE,false;
	setcell 'map$,243,175,243,175,CELL_SHOOTABLE,false;
	setcell 'map$,242,174,242,174,CELL_SHOOTABLE,false;
	//= Fourth Wall
	setcell 'map$,140,178,140,183,CELL_WALKABLE,false;
	setcell 'map$,140,178,140,183,CELL_SHOOTABLE,false;
	//= Last Wall
	setcell 'map$,34,255,43,255,CELL_WALKABLE,false;
	setcell 'map$,34,255,43,255,CELL_SHOOTABLE,false;
			
	disablenpc instance_npcname("Frightened Rgan#19m00");
	disablenpc instance_npcname("Frightened Rgan#19m50");
	disablenpc instance_npcname("Frightened Rgan#19m01");
	disablenpc instance_npcname("Frightened Rgan#19m51");
	disablenpc instance_npcname("Stranded Rgan#19m10");
	disablenpc instance_npcname("Stranded Rgan#19m60");
	disablenpc instance_npcname("Stranded Rgan#19m11");
	disablenpc instance_npcname("Stranded Rgan#19m61");
	disablenpc instance_npcname("Iwin#19m10");
	disablenpc instance_npcname("Iwin#19m11");
	disablenpc instance_npcname("Horuru#19m10");
	disablenpc instance_npcname("Aurelie#19m10");
	disablenpc instance_npcname("Leon#19m10");
	disablenpc instance_npcname("Miriam#19m10");
	disablenpc instance_npcname("Voglinde#19m10");
	disablenpc instance_npcname("Iwin#19m20");
	disablenpc instance_npcname("Iwin#19m21");
	disablenpc instance_npcname("Iwin#19m22");
	disablenpc instance_npcname("Iwin#19m23");
	disablenpc instance_npcname("Horuru#19m20");
	disablenpc instance_npcname("Horuru#19m21");
	disablenpc instance_npcname("Heart Hunter#19m20");
	disablenpc instance_npcname("Heart Hunter#19m21");
	disablenpc instance_npcname("Heart Hunter#19m22");
	disablenpc instance_npcname("Voglinde#19m30");
	disablenpc instance_npcname("Miriam#19m30");
	disablenpc instance_npcname("Heart Hunter#19m30");
	disablenpc instance_npcname("Heart Hunter#19m31");
	disablenpc instance_npcname("Heart Hunter#19m32");
	disablenpc instance_npcname("#e19p01");
	disablenpc instance_npcname("#e19p02");
	disablenpc instance_npcname("#e19p03");
	disablenpc instance_npcname("Injection Device#19m30");
	disablenpc instance_npcname("Injection Device#19m31");
	disablenpc instance_npcname("Injection Device#19m32");
	disablenpc instance_npcname("Injection Device#19m40");
	disablenpc instance_npcname("Heart Hunter#19m40");
	disablenpc instance_npcname("Heart Hunter#19m41");
	disablenpc instance_npcname("Heart Hunter#19m42");
	disablenpc instance_npcname("Leon#19m40");
	disablenpc instance_npcname("Aurelie#19m40");
	disablenpc instance_npcname("Research Device#19m40");
	disablenpc instance_npcname("Research Device#19m41");
	disablenpc instance_npcname("Laboratory Traces#19m40");
	for(.@i = 0; .@i < 7; .@i++)
		disablenpc instance_npcname("Furious Rgan#19m5" + .@i);
	disablenpc instance_npcname("Bagot#19m50");
	disablenpc instance_npcname("Leon#19m60");
	disablenpc instance_npcname("Horuru#19m60");
	disablenpc instance_npcname("Aurelie#19m60");
	disablenpc instance_npcname("Voglinde#19m60");
	disablenpc instance_npcname("Miriam#19m60");
	disablenpc instance_npcname("Central Door#19m61");
	disablenpc instance_npcname("#19m60");
end;

OnEvent01:
	instance_announce false,"Esperem, reunam-se aqui. Precisamos organizar algumas coisas.",bc_map,0x00FFCC;
	'search =0;
	'csn =2;
	disablenpc instance_npcname("Iwin#19m12");
	disablenpc instance_npcname("Iwin#19m13");
	enablenpc instance_npcname("Iwin#19m10");
	enablenpc instance_npcname("Iwin#19m11");
	enablenpc instance_npcname("Horuru#19m10");
	enablenpc instance_npcname("Aurelie#19m10");
	enablenpc instance_npcname("Leon#19m10");
	enablenpc instance_npcname("Miriam#19m10");
	enablenpc instance_npcname("Voglinde#19m10");
end;

OnEvent02:
	'csn =11;
	instance_announce false,"Pessoal, vamos derruba-los.",bc_map,0x00FFCC;
	enablenpc instance_npcname("Heart Hunter#19m40");
	enablenpc instance_npcname("Heart Hunter#19m41");
	enablenpc instance_npcname("Heart Hunter#19m42");
	enablenpc instance_npcname("Leon#19m40");
	enablenpc instance_npcname("Aurelie#19m40");
end;

OnEvent03:
	instance_announce false,"Quem sera o primeiro a chegar?",bc_map,0x00FFCC;
	enablenpc instance_npcname("Heart Hunter#19m50");
	enablenpc instance_npcname("Heart Hunter#19m51");
	enablenpc instance_npcname("Heart Hunter#19m52");
end;
}

1@jorchs,261,34,3	script	Aurelie#19m00	4_EP19_AURELIE,5,5,{
	end;
	
OnTouch:
	if(!is_party_leader()) end;
	if('csn) end;
	'csn =1;
	if (0) {
		npctalk "Nao ha motivo para fingir ser um Rgan aqui.",instance_npcname("Aurelie#19m00");
		sleep 3000;
	}
	npctalk "A unidade Iwin que enviamos ha pouco ja limpou os Rgans ao redor.",instance_npcname("Horuru#19m00");
	sleep 3000;
	npctalk "Agora precisamos avancar e limpar o interior.",instance_npcname("Voglinde#19m00");
	sleep 3000;
	npctalk "O objetivo e proteger Lasgand, Bagot e o Coracao de Ymir.",instance_npcname("Aurelie#19m00");
	sleep 3000;
	npctalk "Temos que entrar todos de uma vez e bloquear a retirada deles. A velocidade sera essencial.",instance_npcname("Horuru#19m00");
	sleep 3000;
	npctalk "So existe uma entrada, mas precisamos verificar todos os lugares caso exista outra que desconhecemos.",instance_npcname("Aurelie#19m00");
	sleep 3000;
	npctalk "Se quisermos descobrir onde eles estao, primeiro precisamos eliminar os Rgans restantes.",instance_npcname("Horuru#19m00");
	sleep 3000;
	npctalk "Segundo o relato da unidade avancada, esses Rgans nao sao comuns. Preparem-se.",instance_npcname("Horuru#19m00");
	sleep 3000;
	npctalk "Cuidado, os Rgans la fora sao bons em usar magia.",instance_npcname("Aurelie#19m00");
	sleep 3000;
	npctalk "Boa sorte, pessoal!",instance_npcname("Horuru#19m00");
	disablenpc instance_npcname("Aurelie#19m00");
	disablenpc instance_npcname("Leon#19m00");
	disablenpc instance_npcname("Miriam#19m00");
	disablenpc instance_npcname("Voglinde#19m00");
	disablenpc instance_npcname("Horuru#19m00");
	disablenpc instance_npcname("Iwin#19m00");
	disablenpc instance_npcname("Iwin#19m01");
	enablenpc instance_npcname("Frightened Rgan#19m00");
	enablenpc instance_npcname("Frightened Rgan#19m50");
	enablenpc instance_npcname("Frightened Rgan#19m01");
	enablenpc instance_npcname("Frightened Rgan#19m51");
	donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon";
end;
}

1@jorchs,260,35,3	duplicate(dummy_csn)	Leon#19m00	4_EP19_LEON
1@jorchs,256,34,5	duplicate(dummy_csn)	Miriam#19m00	4_EP18_MIRIAM
1@jorchs,257,35,5	duplicate(dummy_csn)	Voglinde#19m00	4_EP19_VOGLINDE
1@jorchs,258,32,7	duplicate(dummy_csn)	Horuru#19m00	4_M_REPAIR
1@jorchs,256,31,7	duplicate(dummy_csn)	Iwin#19m00	4_M_REPAIR
1@jorchs,259,31,7	duplicate(dummy_csn)	Iwin#19m01	4_M_REPAIR

1@jorchs,143,60,5	script	Iwin#19m11	4_M_REPAIR,5,5,{
	end;
	
OnTouch:
	if(!is_party_leader()) end;
	if('csn == 2){
		'csn =3;
		npctalk "O que aconteceu com os Rgans?! Nunca vi um Rgan assim na minha vida!",instance_npcname("Horuru#19m10");
		sleep 2500;
		npctalk "Acho que sao Rgans de grau intermediario, mas nao consegui reconhecer a forma deles.",instance_npcname("Leon#19m10");
		sleep 2500;
		npctalk "Por que um Rgan intermediario ficaria assim? Esse nao seria um superior?",instance_npcname("Aurelie#19m10");
		sleep 2500;
		npctalk "Acho que tem relacao com as Ilusoes. Parece que eles foram modificados.",instance_npcname("Miriam#19m10");
		sleep 2500;
		npctalk "Modificar nao deveria deixa-los melhores? Por mais que eu olhe...",instance_npcname("Horuru#19m10");
		sleep 2500;
		npctalk "Ou foram envolvidos nisso, ou seriam descartados depois de usados.",instance_npcname("Leon#19m10");
		sleep 2500;
		npctalk "Isso e incomum. Vamos avancar.",instance_npcname("Voglinde#19m10");
		sleep 3000;
		npctalk "O lugar e complicado, entao e melhor nos separarmos para procurar.",instance_npcname("Horuru#19m10");
		sleep 3000;
		npctalk "Os Rgans reunidos por Lasgand provavelmente estao na residencia dele.",instance_npcname("Miriam#19m10");
		sleep 3000;
		npctalk "Bagot tambem estava trabalhando em algo, entao devemos verificar a residencia e os laboratorios das Ilusoes.",instance_npcname("Horuru#19m10");
		sleep 2500;
		npctalk "Fica na parte mais profunda deste lugar.",instance_npcname("Leon#19m10");
		sleep 2500;
		npctalk "Vamos?",instance_npcname("Aurelie#19m10");
		setcell 'map$,139,59,139,59,CELL_WALKABLE,true;
		setcell 'map$,138,58,138,58,CELL_WALKABLE,true;
		setcell 'map$,137,57,137,57,CELL_WALKABLE,true;
		setcell 'map$,139,59,139,59,CELL_SHOOTABLE,true;
		setcell 'map$,138,58,138,58,CELL_SHOOTABLE,true;
		setcell 'map$,137,57,137,57,CELL_SHOOTABLE,true;		
		disablenpc instance_npcname("Iwin#19m10");
		disablenpc instance_npcname("Iwin#19m11");
		disablenpc instance_npcname("Horuru#19m10");
		disablenpc instance_npcname("Aurelie#19m10");
		disablenpc instance_npcname("Leon#19m10");
		disablenpc instance_npcname("Miriam#19m10");
		disablenpc instance_npcname("Voglinde#19m10");
		enablenpc instance_npcname("Iwin#19m21");
		enablenpc instance_npcname("Stranded Rgan#19m10");
		enablenpc instance_npcname("Stranded Rgan#19m60");
		enablenpc instance_npcname("Stranded Rgan#19m11");
		enablenpc instance_npcname("Stranded Rgan#19m61");
		donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon";
	}
end;
}

1@jorchs,142,58,5	duplicate(Iwin#19m11)	Horuru#19m10	4_M_REPAIR,5,5
1@jorchs,145,59,3	duplicate(dummy_csn)	Aurelie#19m10	4_EP19_AURELIE
1@jorchs,146,58,3	duplicate(dummy_csn)	Leon#19m10	4_EP19_LEON
1@jorchs,145,56,1	duplicate(dummy_csn)	Miriam#19m10	4_EP18_MIRIAM
1@jorchs,143,56,1	duplicate(dummy_csn)	Voglinde#19m10	4_EP19_VOGLINDE
1@jorchs,140,57,5	duplicate(dummy_csn)	Iwin#19m10	4_M_REPAIR
1@jorchs,140,56,5	duplicate(dummy_csn)	Iwin#19m12	4_M_REPAIR

1@jorchs,140,58,5	script	Iwin#19m13	4_M_REPAIR,5,5,{
	end;
	
OnTouch:
	npctalk "Nenhum grupo de busca foi enviado alem desta area ainda.",instance_npcname("Iwin#19m12");
	sleep 2000;
	npctalk "Vamos reunir informacoes aqui e entrar, entao concentrem-se na busca ao redor.",instance_npcname("Iwin#19m13");
end;
}

1@jorchs,114,119,3	script	Iwin#19m21	4_M_REPAIR,3,3,{
	if('csn < 4)
		npctalk "Voce chegou! Esses Rgans sao impenetraveis, nao atacam e sao estranhos. Vamos achar um jeito, entao foque na busca!",instance_npcname("Iwin#19m21");
	end;
}

1@jorchs,112,119,5	script	Horuru#19m20	4_M_REPAIR,5,5,{
	end;
	
OnTouch:
	if(!is_party_leader()) end;
	if('csn == 4){
		'csn =5;
		npctalk "Podemos simplesmente continuar!",instance_npcname("Horuru#19m20");
		sleep 2000;
		npctalk "Espere!! Ha um monte de Rgans na nossa frente!",instance_npcname("Iwin#19m20");
		sleep 2000;
		npctalk "Eu vou atravessa-los!",instance_npcname("Horuru#19m20");
		sleep 1500;
		hideonnpc instance_npcname("Horuru#19m20");
		hideonnpc instance_npcname("Iwin#19m20");
		hideonnpc instance_npcname("Iwin#19m21");
		sleep 1500;
		enablenpc instance_npcname("Horuru#19m21");
		enablenpc instance_npcname("Iwin#19m22");
		enablenpc instance_npcname("Iwin#19m23");
		sleep 2000;
		specialeffect EF_HYOUSYOURAKU,AREA,instance_npcname("Horuru#19m21");
		specialeffect EF_HYOUSYOURAKU,AREA,instance_npcname("Iwin#19m22");
		specialeffect EF_HYOUSYOURAKU,AREA,instance_npcname("Iwin#19m23");
		specialeffect EF_HFLIMOON3,AREA,instance_npcname("Horuru#19m21");
		specialeffect EF_HFLIMOON3,AREA,instance_npcname("Iwin#19m22");
		specialeffect EF_HFLIMOON3,AREA,instance_npcname("Iwin#19m23");
		specialeffect EF_M03,AREA,instance_npcname("Horuru#19m21");
		specialeffect EF_M03,AREA,instance_npcname("Iwin#19m22");
		specialeffect EF_M03,AREA,instance_npcname("Iwin#19m23");
		npctalk "?!!!",instance_npcname("Horuru#19m21");
		sleep 1500;
		disablenpc instance_npcname("Horuru#19m21");
		disablenpc instance_npcname("Iwin#19m22");
		disablenpc instance_npcname("Iwin#19m23");
		sleep 1500;
		hideoffnpc instance_npcname("Horuru#19m20");
		hideoffnpc instance_npcname("Iwin#19m20");
		hideoffnpc instance_npcname("Iwin#19m21");
		npctalk "Nao consigo atravessar! Sao muitos!!! Como vamos passar por isso?",instance_npcname("Horuru#19m20");
		sleep 2000;
		npctalk "Isso nao parece estranho?",instance_npcname("Iwin#19m20");
		sleep 2500;
		npctalk "Estamos aqui ha um tempo, mas eles nao nos atacam!",instance_npcname("Iwin#19m21");
		sleep 2000;
		enablenpc instance_npcname("Heart Hunter#19m20");
		npctalk "So preciso ligar isto... o que? Eles ja chegaram?",instance_npcname("Heart Hunter#19m20");
		sleep 2000;
		npctalk "Estamos atrasados? Bem, nao importa.",instance_npcname("Heart Hunter#19m20");
		sleep 2000;
		npctalk "Quem e aquele? Sao os Heart Hunters que as Ilusoes estao criando?",instance_npcname("Horuru#19m20");
		sleep 2000;
		npctalk "Esta e a 5a linha de defesa, solicitando apoio.",instance_npcname("Heart Hunter#19m20");
		sleep 2000;
		npctalk "Ha invasores. Esta dificil continuar expandindo a linha de defesa por causa dos obstaculos. Repito: solicitando apoio.",instance_npcname("Heart Hunter#19m20");
		sleep 2000;
		npctalk "Chamando reforcos!!",instance_npcname("Horuru#19m20");
		sleep 2000;
		enablenpc instance_npcname("Heart Hunter#19m21");
		enablenpc instance_npcname("Heart Hunter#19m22");
		npctalk "Qual e o obstaculo?",instance_npcname("Heart Hunter#19m22");
		sleep 2000;
		npctalk "Como pode ver, esse aventureiro ja esta aqui.",instance_npcname("Heart Hunter#19m20");
		sleep 2000;
		npctalk "O que e isto? Esses Rgans estao mesmo combinados? E exatamente como Bagot disse.",instance_npcname("Heart Hunter#19m21");
		sleep 2000;
		npctalk "Eu sei. Rgans bem criados e fortes conseguem bloquear a entrada sozinhos!",instance_npcname("Heart Hunter#19m20");
		sleep 2000;
		npctalk "De qualquer forma, por que tem tanta gente? Nao podemos simplesmente elimina-los?",instance_npcname("Heart Hunter#19m22");
		sleep 2000;
		npctalk "Tentei ativar o dispositivo e criar uma barreira, mas eles ja chegaram.",instance_npcname("Heart Hunter#19m20");
		sleep 2000;
		npctalk "Va operar o dispositivo. Nos cuidamos deles.",instance_npcname("Heart Hunter#19m20");
		npctalk "Entendido.",instance_npcname("Heart Hunter#19m22");
		sleep 2000;
		disablenpc instance_npcname("Heart Hunter#19m22");
		npctalk "Huh?! Eles estao se separando? Aventureiro! Nos cuidamos daquele ali, cuide deste aqui!",instance_npcname("Horuru#19m20");
		sleep 2000;
		hideonnpc instance_npcname("Horuru#19m20");
		hideonnpc instance_npcname("Iwin#19m20");
		hideonnpc instance_npcname("Iwin#19m21");
		hideonnpc instance_npcname("Heart Hunter#19m20");
		disablenpc instance_npcname("Heart Hunter#19m21");
		donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon01";
		end;
	}
end;
}

1@jorchs,112,116,7	duplicate(dummy_csn)	Iwin#19m20	4_M_REPAIR
1@jorchs,119,123,3	duplicate(dummy_csn)	Heart Hunter#19m20	21536
1@jorchs,116,124,3	duplicate(dummy_csn)	Heart Hunter#19m21	21536
1@jorchs,123,124,3	duplicate(dummy_csn)	Heart Hunter#19m22	21536
1@jorchs,120,129,1	duplicate(dummy_csn)	Twisted Rgan#19m20	21535
1@jorchs,120,127,3	duplicate(dummy_csn)	Crushed Rgan#19m21	21535
1@jorchs,122,127,3	duplicate(dummy_csn)	Howling Rgan#19m22	21535
1@jorchs,123,127,1	duplicate(dummy_csn)	Modified Superior Rgan#19m23	21534
1@jorchs,124,126,3	duplicate(dummy_csn)	Agonizing Rgan#19m24	21535
1@jorchs,125,125,3	duplicate(dummy_csn)	Deformed Rgan#19m25	21535
1@jorchs,126,125,3	duplicate(dummy_csn)	Twisted Rgan#19m26	21535

1@jorchs,118,127,7	duplicate(dummy_csn)	Horuru#19m21	4_M_REPAIR
1@jorchs,122,126,7	duplicate(dummy_csn)	Iwin#19m22	4_M_REPAIR
1@jorchs,124,124,7	duplicate(dummy_csn)	Iwin#19m23	4_M_REPAIR

1@jorchs,244,176,0	script	#e19p04	HIDDEN_WARP_NPC,7,7,{
	end;
	
OnTouch:
	if('csn < 7){
		unittalk getcharid(3),strcharinfo(0) + " : Esta parede de Rgans... nao consigo atacar nem atravessar? Vou verificar de novo depois da busca.";
		end;
	}
	if('csn == 10){
		viewpoint 2,219,148,1,0xFFFF33;
		viewpoint 1,226,244,2,0xFFFF33;
		viewpoint 1,191,203,3,0xFFFF33;
	}
end;
}

1@jorchs,251,171,1	script	Voglinde#19m30	4_EP19_VOGLINDE,5,5,{
	end;
	
OnTouch:
	if('csn == 7){
		'csn =8;
		npctalk "Aventureiro! Voce esta bem.",instance_npcname("Miriam#19m30");
		sleep 2000;
		npctalk "Os Rgans na nossa frente estao sob controle dos Heart Hunters, certo?",instance_npcname("Voglinde#19m30");
		sleep 2500;
		npctalk "Como eles podem usar um aliado como barreira?",instance_npcname("Miriam#19m30");
		sleep 2000;
		npctalk "Se eles tivessem esse tipo de consciencia, nem teriam criado os Heart Hunters.",instance_npcname("Voglinde#19m30");
		sleep 2000;
		enablenpc instance_npcname("Heart Hunter#19m30");
		enablenpc instance_npcname("Heart Hunter#19m31");
		enablenpc instance_npcname("Heart Hunter#19m32");
		npctalk "Estao falando de nos? Nao se preocupem. Fazemos isso porque queremos.",instance_npcname("Heart Hunter#19m30");
		sleep 2500;
		npctalk "O mesmo vale para os Rgans superiores.",instance_npcname("Heart Hunter#19m31");
		sleep 2000;
		npctalk "Todos se alinharam porque era a chance deles de serem uteis.",instance_npcname("Heart Hunter#19m32");
		sleep 2500;
		npctalk "Os Rgans intermediarios... o que aconteceu com eles?",instance_npcname("Voglinde#19m30");
		sleep 2000;
		npctalk "Bem... nao ha muito o que fazer por aqueles intermediarios.",instance_npcname("Heart Hunter#19m30");
		sleep 2000;
		npctalk "E errado estar em um lugar perigoso na hora errada...?",instance_npcname("Heart Hunter#19m32");
		sleep 2000;
		npctalk "Onde estao todos? O que voces estao fazendo juntos?",instance_npcname("Miriam#19m30");
		sleep 2000;
		npctalk "Nao tenho motivo para te contar isso. Nao sou burro.",instance_npcname("Heart Hunter#19m32");
		sleep 2000;
		npctalk "Entao saia do nosso caminho e pare de falar besteira.",instance_npcname("Miriam#19m30");
		sleep 2500;
		npctalk "Acho que o povo de Arunafeltz e bem direto.",instance_npcname("Heart Hunter#19m31");
		npctalk "Nao podemos deixar voces passarem. Tambem estamos cumprindo ordens.",instance_npcname("Heart Hunter#19m30");
		sleep 2500;
		npctalk "Para impedir voces. Eu preparei algo... Uh...",instance_npcname("Heart Hunter#19m32");
		sleep 2500;
		npctalk "Entendi, voce esta ficando sem tempo, nao esta?",instance_npcname("Voglinde#19m30");
		sleep 2500;
		npctalk "Como voce sabia? Era para durar um pouco mais, mas nao sei o que aconteceu! Vamos!!!",instance_npcname("Heart Hunter#19m30");
		sleep 2000;
		specialeffect EF_SCREEN_QUAKE ,AREA,instance_npcname("Heart Hunter#19m32");
		npctalk "Disseram para eu usar isto em momentos assim. Mas acho que nunca vi eles usando antes?",instance_npcname("Heart Hunter#19m30");
		instance_announce false,"O dispositivo de injecao foi ativado.",bc_map,0x00FFCC;
		sleep 2500;
		npctalk "Dizem que ele choca um ovo superior e o transforma em um Rgan Superior Modificado em instantes.",instance_npcname("Heart Hunter#19m32");
		specialeffect EF_CLOUD4 ,AREA,instance_npcname("Heart Hunter#19m32");
		sleep 2000;
		npctalk "Isso vai fazer nascer um monte de Rgans?",instance_npcname("Heart Hunter#19m31");
		sleep 2500;
		npctalk "Enquanto o suprimento de mana continuar, ele continuara criando Rgans.",instance_npcname("Heart Hunter#19m32");
		sleep 2500;
		npctalk "Para mim, isso significa que podemos cortar o suprimento de mana, certo? Voce e mais gentil do que eu pensava.",instance_npcname("Voglinde#19m30");
		emotion ET_HUK,getnpcid(0,instance_npcname("Heart Hunter#19m30"));
		emotion ET_HUK,getnpcid(0,instance_npcname("Heart Hunter#19m31"));
		emotion ET_HUK,getnpcid(0,instance_npcname("Heart Hunter#19m32"));
		sleep 2500;
		npctalk "Bem, acha que vamos deixar? Alem disso, eu tambem chamei reforcos!!",instance_npcname("Heart Hunter#19m31");
		sleep 2000;
		npctalk "Vamos para o proximo objetivo!",instance_npcname("Heart Hunter#19m30");
		sleep 2000;
		disablenpc instance_npcname("Heart Hunter#19m30");
		disablenpc instance_npcname("Heart Hunter#19m31");
		disablenpc instance_npcname("Heart Hunter#19m32");
		npctalk "Aventureiro, vamos persegui-los. Elimine os Rgans e pare o dispositivo...!",instance_npcname("Voglinde#19m30");
		sleep 2000;
		disablenpc instance_npcname("Voglinde#19m30");
		disablenpc instance_npcname("Miriam#19m30");
		enablenpc instance_npcname("#e19p01");
		donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon03";
		end;
	}
end;
}

1@jorchs,249,170,1	duplicate(dummy_csn)	Miriam#19m30	4_EP18_MIRIAM

1@jorchs,249,177,1	duplicate(dummy_csn)	Modified Superior Rgan#19m30	21534
1@jorchs,248,176,3	duplicate(dummy_csn)	Twisted Rgan#19m31	21535
1@jorchs,247,176,3	duplicate(dummy_csn)	Combined Rgan#19m32	21535
1@jorchs,246,176,1	duplicate(dummy_csn)	Modified Superior Rgan#19m33	21534
1@jorchs,245,176,3	duplicate(dummy_csn)	Modified Superior Rgan#19m34	21534
1@jorchs,244,176,3	duplicate(dummy_csn)	Crushed Rgan#19m35	21535
1@jorchs,242,174,3	duplicate(dummy_csn)	Twisted Rgan#19m37	21535
1@jorchs,230,160,0	duplicate(dummy_csn)	#e19c01	HIDDEN_WARP_NPC

1@jorchs,248,173,5	duplicate(dummy_csn)	Heart Hunter#19m30	21536
1@jorchs,246,173,5	duplicate(dummy_csn)	Heart Hunter#19m31	21536
1@jorchs,243,173,5	duplicate(dummy_csn)	Heart Hunter#19m32	21536

1@jorchs,243,166,0	script	#e19p01	HIDDEN_WARP_NPC,7,7,{
	end;
	
OnTouch:
	if('csn < 9)
		viewpoint 1,219,148,1,0xFFFF33;
end;
}

1@jorchs,219,148,5	script	Injection Device#19m30	4_ENERGY_BLUE,3,3,{
	if('csn == 8){
		disablenpc instance_npcname(strnpcinfo(0));
		viewpoint 2,219,148,1,0xFFFF33;
		'csn =9;
		npctalk "Dispositivo de injecao desativado";
		specialeffect EF_AGIUP,AREA,instance_npcname("#e19c01");
		specialeffect EF_GUARD2,AREA,instance_npcname("#e19c01");
		donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon04";
	}
	end;
}

1@jorchs,226,244,7	script	Injection Device#19m31	4_ENERGY_BLUE,{
	if(!'dev_1) end;
	'dev_1 =0;
	npctalk "Dispositivo de injecao desativado";
	disablenpc instance_npcname(strnpcinfo(0));
	if(!'dev_2){
		if(!'clear)
			instance_announce false,"Ainda ha Rgans por ali!",bc_map,0x00FFCC;
		else
			donpcevent instance_npcname("#191_csn_control") + "::" + "OnEvent02";
	}
	end;
	
OnSummon:
	while('dev_1){
		if(!'dev_1) end;
		specialeffect EF_AGIUP,AREA,instance_npcname("#e19c02");
		specialeffect EF_GUARD2,AREA,instance_npcname("#e19c02");
		monster 'map$,239,255,"Unstable Modified Rgan",21534,1;
		sleep 30000;
	}
end;
}

1@jorchs,239,255,0	duplicate(dummy_csn)	#e19c02	HIDDEN_WARP_NPC

1@jorchs,191,203,7	script	Injection Device#19m32	4_ENERGY_BLUE,{
	if(!'dev_2) end;
	'dev_2 =0;
	npctalk "Dispositivo de injecao desativado";
	disablenpc instance_npcname(strnpcinfo(0));
	if(!'dev_1){
		if(!'clear)
			instance_announce false,"Ainda ha Rgans por ali!",bc_map,0x00FFCC;
		else
			donpcevent instance_npcname("#191_csn_control") + "::" + "OnEvent02";
	}
	end;
	
OnSummon:
	while('dev_2){
		if(!'dev_2) end;
		specialeffect EF_AGIUP,AREA,instance_npcname("#e19c03");
		specialeffect EF_GUARD2,AREA,instance_npcname("#e19c03");
		monster 'map$,198,205,"Unstable Modified Rgan",21534,1;
		sleep 30000;
	}
end;
}

1@jorchs,198,205,0	duplicate(dummy_csn)	#e19c03	HIDDEN_WARP_NPC

1@jorchs,192,206,5	script	Injection Device#19m40	4_ENERGY_BLUE,3,3,{
	if(!'dev_3) end;
	'dev_3 =0;
	npctalk "Dispositivo liberado";
	disablenpc instance_npcname(strnpcinfo(0));
	donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon05";
	end;
	
OnSummon:
	while('dev_3){
		if(!'dev_3) end;
		specialeffect EF_AGIUP,AREA,instance_npcname("#e19c04");
		specialeffect EF_GUARD2,AREA,instance_npcname("#e19c04");
		monster 'map$,199,197,"Unstable Modified Rgan",21534,1;
		sleep 30000;
	}
end;
}

1@jorchs,199,197,0	duplicate(dummy_csn)	#e19c04	HIDDEN_WARP_NPC


1@jorchs,20,195,7	script	Research Device#19m40	4_ENERGY_BLUE,{
	if(!'dev_4) end;
	'dev_4 =0;
	disablenpc instance_npcname(strnpcinfo(0));
	donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon08";
	unittalk getcharid(3),strcharinfo(0) + " : Coracao de Ymir... Claro que nao... Nao vejo nada util.";
	instance_announce false,"Ainda ha muitos Rgans para lidar!",bc_map,0x00FFCC;
	end;
	
OnSummon:
	while('dev_4){
		if(!'dev_4) end;
		specialeffect EF_AGIUP,AREA,instance_npcname("#e19c05");
		specialeffect EF_GUARD2,AREA,instance_npcname("#e19c05");
		monster 'map$,17,188,"Unstable Modified Rgan",21534,1;
		sleep 30000;
	}
end;
}

1@jorchs,17,188,0	duplicate(dummy_csn)	#e19c05	HIDDEN_WARP_NPC

1@jorchs,24,142,7	script	Research Device#19m41	4_ENERGY_BLUE,{
	if(!'dev_5) end;
	'dev_5 =0;
	disablenpc instance_npcname(strnpcinfo(0));
	donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon07";
	unittalk getcharid(3),strcharinfo(0) + " : Isto... e importante? Nao sei. Ele levou todos os materiais uteis?";
	end;
	
OnSummon:
	while('dev_5){
		if(!'dev_5) end;
		specialeffect EF_AGIUP,AREA,instance_npcname("#e19c06");
		specialeffect EF_GUARD2,AREA,instance_npcname("#e19c06");
		monster 'map$,17,133,"Unstable Modified Rgan",21534,1;
		sleep 30000;
	}
end;
}

1@jorchs,17,133,0	duplicate(dummy_csn)	#e19c06	HIDDEN_WARP_NPC


1@jorchs,16,135,3	script	Laboratory Traces#19m40	4_ENERGY_BLUE,{
	unittalk getcharid(3),strcharinfo(0) + " : Sobrou um pouco.";
	end;
}


1@jorchs,156,177,0	script	#e19p02	HIDDEN_WARP_NPC,7,7,{
	end;
	
OnTouch:
	if('csn == 12 || 'csn == 13){
		viewpoint 1,192,206,1,0xFFFF33;
		viewpoint 2,226,244,2,0xFFFF33;
		viewpoint 2,191,203,3,0xFFFF33;
	}
end;
}

1@jorchs,137,181,0	script	#e19p03	HIDDEN_WARP_NPC,7,7,{
	end;
	
OnTouch:
	if('csn < 18){
		viewpoint 2,192,206,1,0xFFFF33;
		viewpoint 1,20,195,2,0xFFFF33;
		viewpoint 1,24,142,3,0xFFFF33;
	}
end;
}

1@jorchs,150,177,1	script	Leon#19m40	4_EP19_LEON,3,3,{
	if('csn == 11){
		'csn =12;
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Heart Hunter#19m40");
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Heart Hunter#19m41");
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Heart Hunter#19m42");
		npctalk "O que ha com voces? Sao voces que estao causando confusao porque nao conseguem entrar?",instance_npcname("Aurelie#19m40");
		sleep 3000;
		npctalk "Nao estou tentando insinuar nada, so nao entendo.",instance_npcname("Aurelie#19m40");
		sleep 3000;
		npctalk "Aventureiro, voce chegou? Nos...",instance_npcname("Leon#19m40");
		instance_announce false,"O dispositivo de injecao foi ativado.",bc_map,0x00FFCC;
		sleep 3000;
		npctalk "Vamos segura-los, entao encontre o dispositivo primeiro e pare-o.",instance_npcname("Aurelie#19m40");
		sleep 2000;
		npctalk "Eles mexeram nele e talvez tenham reiniciado?",instance_npcname("Leon#19m40");
		sleep 2000;
		npctalk "Vamos conversar sem eles, pessoal.",instance_npcname("Heart Hunter#19m40");
		sleep 2000;
		npctalk "O que e isto? Por que nao consigo me mover?",instance_npcname("Heart Hunter#19m41");
		sleep 2000;
		npctalk "E porque eu sou incrivel.",instance_npcname("Aurelie#19m40");
		sleep 2500;
		npctalk "Agora, vamos terminar nossa conversa?",instance_npcname("Leon#19m40");
		sleep 3000;
		npctalk "Deixe este lugar conosco.",instance_npcname("Aurelie#19m40");
		npctalk "Nao fique ai parado, pare o aventureiro!",instance_npcname("Heart Hunter#19m42");
		enablenpc instance_npcname("#e19p02");
		'dev_3 =1;
		enablenpc instance_npcname("Injection Device#19m40");
		donpcevent instance_npcname("Injection Device#19m40") + "::" + "OnSummon";
		'csn =13;
		end;
	}
	if('csn == 13){
		npctalk "Deixe este lugar conosco e pare o dispositivo.",instance_npcname("Aurelie#19m40");
		end;
	}
	if('csn == 14){
		'csn =15;
		npctalk "Aqui. Eles estao reunidos la dentro; precisamos avancar.",instance_npcname("Aurelie#19m40");
		sleep 2000;
		npctalk "Eles chamaram todos os Rgans de grau superior para a residencia de Lasgand.",instance_npcname("Aurelie#19m40");
		sleep 2000;
		npctalk "Po... por favor, nao conte que fomos nos que dissemos isso. Por favor.",instance_npcname("Heart Hunter#19m40");
		sleep 2000;
		npctalk "Ouvi dizer que so ha uma entrada. Isso e verdade?",instance_npcname("Leon#19m40");
		npctalk "Nos so dissemos a verdade. Por favor, acredite em mim.",instance_npcname("Heart Hunter#19m41");
		sleep 3000;
		npctalk "De qualquer forma, tenho certeza de que o que Lasgand e Bagot querem esta logo adiante.",instance_npcname("Aurelie#19m40");
		sleep 2000;
		npctalk "Precisamos nos apressar.",instance_npcname("Aurelie#19m40");
		sleep 1500;
		npctalk "Pessoal, por favor, deixem eu e eles fora disso. Contamos tudo o que sabiamos.",instance_npcname("Heart Hunter#19m42");
		sleep 1500;
		npctalk "Alem disso...",instance_npcname("Leon#19m40");
		emotion ET_SURPRISE,getnpcid(0,instance_npcname("Leon#19m40"));
		specialeffect EF_SCREEN_QUAKE,AREA,instance_npcname("Leon#19m40");
		instance_announce false,"O amplificador de mana foi energizado.",bc_map,0x00FFCC;
		sleep 3000;
		npctalk "Oh? O fluxo de mana esta estranho?",instance_npcname("Leon#19m40");
		sleep 2000;
		npctalk "Sim. Acho que os outros nao vao conseguir lidar com isto.",instance_npcname("Aurelie#19m40");
		sleep 2500;
		npctalk "Aventureiro, podemos deixar isto com voce?",instance_npcname("Leon#19m40");
		sleep 3000;
		npctalk "Isso e incomum. E melhor nos apressarmos e cuidar disso. Vou entrar primeiro.",instance_npcname("Leon#19m40");
		sleep 2000;
		disablenpc instance_npcname("Leon#19m40");
		npctalk "Ha! A proposito! Voces nao estao nos subestimando?",instance_npcname("Heart Hunter#19m40");
		sleep 2000;
		npctalk "Se o irmao assustador desaparecer, nos tambem conseguimos!",instance_npcname("Heart Hunter#19m41");
		sleep 2000;
		npctalk "E melhor fechar essa boca. Entao, por favor.",instance_npcname("Aurelie#19m40");
		disablenpc instance_npcname("Aurelie#19m40");
		sleep 500;
		disablenpc instance_npcname("Heart Hunter#19m40");
		disablenpc instance_npcname("Heart Hunter#19m41");
		disablenpc instance_npcname("Heart Hunter#19m42");
		donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon06";
		end;
	}
	end;
}

1@jorchs,151,179,1	duplicate(Leon#19m40)	Aurelie#19m40	4_EP19_AURELIE,5,5

1@jorchs,149,179,5	duplicate(dummy_csn)	Heart Hunter#19m40	21536
1@jorchs,149,178,5	duplicate(dummy_csn)	Heart Hunter#19m41	21536
1@jorchs,150,180,5	duplicate(dummy_csn)	Heart Hunter#19m42	21536


1@jorchs,140,183,1	duplicate(dummy_csn)	Modified Superior Rgan#19m40	21534
1@jorchs,140,182,3	duplicate(dummy_csn)	Crushed Rgan#19m41	21535

1@jorchs,139,181,3	script	Modified Superior Rgan#19m42	21534,5,5,{
	end;
	
OnTouch:
	if('csn < 11)
		unittalk getcharid(3),strcharinfo(0) + " : Nao consigo passar pela parede de Rgans. Tenho que terminar a busca primeiro.";
end;
}

1@jorchs,139,180,1	duplicate(dummy_csn)	Modified Superior Rgan#19m43	21534
1@jorchs,138,179,3	duplicate(dummy_csn)	Twisted Rgan#19m44	21535
1@jorchs,138,178,3	duplicate(dummy_csn)	Modified Superior Rgan#19m45	21534

1@jorchs,39,251,5	script	Heart Hunter#19m50	21536,5,5,{
	end;
	
OnTouch:
	if('csn == 18){
		'csn =19;
		npctalk "Quem e?",instance_npcname("Heart Hunter#19m50");
		sleep 2000;
		npctalk "Aventureiro?",instance_npcname("Heart Hunter#19m52");
		sleep 2000;
		npctalk "O aventureiro foi o primeiro a chegar?",instance_npcname("Heart Hunter#19m51");
		sleep 1500;
		npctalk "Estavamos nos perguntando quem chegaria primeiro.",instance_npcname("Heart Hunter#19m52");
		npctalk "Eu venci.",instance_npcname("Heart Hunter#19m50");
		sleep 2500;
		enablenpc instance_npcname("Bagot#19m50");
		npctalk "So o aventureiro veio?",instance_npcname("Bagot#19m50");
		sleep 2000;
		npctalk "Sim. Os outros parecem estar lidando com os Rgans.",instance_npcname("Heart Hunter#19m50");
		sleep 2000;
		npctalk "Mesmo? Voce queria muito me encontrar, aventureiro?",instance_npcname("Bagot#19m50");
		sleep 3000;
		npctalk "Achou que conseguiria me enganar com aquele truque bonitinho outro dia?!",instance_npcname("Bagot#19m50");
		sleep 3000;
		npctalk "Tenho um presente preparado. Quero dizer, isto aqui.",instance_npcname("Bagot#19m50");
		sleep 3000;
		npctalk "Parece que os outros estao vindo. O caminho se cruzou.",instance_npcname("Bagot#19m50");
		enablenpc instance_npcname("Furious Rgan#19m50");
		enablenpc instance_npcname("Furious Rgan#19m51");
		enablenpc instance_npcname("Furious Rgan#19m52");
		enablenpc instance_npcname("Furious Rgan#19m53");
		enablenpc instance_npcname("Furious Rgan#19m55");
		enablenpc instance_npcname("Furious Rgan#19m56");
		npctalk "Ugh---",instance_npcname("Furious Rgan#19m50");
		npctalk "Grr--- rrrr---",instance_npcname("Furious Rgan#19m51");
		npctalk "Ah... nao... consigo parar...",instance_npcname("Furious Rgan#19m52");
		npctalk ".........",instance_npcname("Furious Rgan#19m53");
		npctalk "Ugh... Oh.. .Uh...",instance_npcname("Furious Rgan#19m54");
		npctalk "Uh Uh Uh Uh--- Uh---",instance_npcname("Furious Rgan#19m55");
		npctalk "Onde... isto...",instance_npcname("Furious Rgan#19m56");
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m50");
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m51");
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m52");
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m53");
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m55");
		specialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m56");
		specialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m50");
		specialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m51");
		specialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m52");
		specialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m53");
		specialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m55");
		specialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m56");
		sleep 3000;
		npctalk "Mas isso nao importa. Fico feliz por poder entrega-lo pessoalmente.",instance_npcname("Bagot#19m50");
		sleep 3000;
		npctalk "Voce deve estar se perguntando o que vou fazer, certo? Deve ter ficado tao curioso que agiu por impulso.",instance_npcname("Bagot#19m50");
		sleep 3000;
		npctalk "Mas nao vou cair num truque tao raso.",instance_npcname("Bagot#19m50");
		sleep 3000;
		npctalk "Vou dizer diretamente agora. Fiz com que me seguissem, por isso deixei rastros desnecessarios.",instance_npcname("Bagot#19m50");
		sleep 2000;
		npctalk "E um pouco irritante pensar que moscas vao grudar no novo mundo que vou criar.",instance_npcname("Bagot#19m50");
		sleep 3000;
		npctalk "E Lasgand tambem nao esta muito irritado com os humanos que o enganaram?",instance_npcname("Bagot#19m50");
		sleep 3000;
		npctalk "Bem, ja que e um presente meu, vou me retirar por aqui.",instance_npcname("Bagot#19m50");
		sleep 500;
		disablenpc instance_npcname("Bagot#19m50");
		donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon09";
		end;
	}
end;
}

1@jorchs,36,251,5	duplicate(dummy_csn)	Heart Hunter#19m51	21536
1@jorchs,41,253,5	duplicate(dummy_csn)	Heart Hunter#19m52	21536

1@jorchs,38,255,3	script	Modified Superior Rgan#19m54	21534,5,5,{
	end;
	
OnTouch:
	if('csn < 18)
		unittalk getcharid(3),strcharinfo(0) + " : Ha uma parede de Rgans. Terei que voltar aqui depois de procurar.";
end;
}

1@jorchs,38,256,5	duplicate(Modified Superior Rgan#19m54)	Modified Superior Rgan#19m64	21534,5,5

1@jorchs,34,255,1	duplicate(dummy_csn)	Modified Superior Rgan#19m50	21534
1@jorchs,35,255,3	duplicate(dummy_csn)	Modified Superior Rgan#19m51	21534
1@jorchs,36,255,3	duplicate(dummy_csn)	Modified Superior Rgan#19m52	21534
1@jorchs,37,255,1	duplicate(dummy_csn)	Modified Superior Rgan#19m53	21534
1@jorchs,39,255,3	duplicate(dummy_csn)	Modified Superior Rgan#19m55	21534
1@jorchs,40,255,1	duplicate(dummy_csn)	Modified Superior Rgan#19m56	21534
1@jorchs,41,255,3	duplicate(dummy_csn)	Modified Superior Rgan#19m57	21534
1@jorchs,42,255,3	duplicate(dummy_csn)	Modified Superior Rgan#19m58	21534
1@jorchs,43,255,3	duplicate(dummy_csn)	Modified Superior Rgan#19m59	21534
1@jorchs,35,256,5	duplicate(dummy_csn)	Modified Superior Rgan#19m61	21534
1@jorchs,37,256,1	duplicate(dummy_csn)	Modified Superior Rgan#19m63	21534
1@jorchs,39,256,3	duplicate(dummy_csn)	Modified Superior Rgan#19m65	21534
1@jorchs,41,256,1	duplicate(dummy_csn)	Modified Superior Rgan#19m67	21534
1@jorchs,42,256,5	duplicate(dummy_csn)	Modified Superior Rgan#19m68	21534

1@jorchs,31,249,5	duplicate(dummy_csn)	Furious Rgan#19m50	21590
1@jorchs,47,237,1	duplicate(dummy_csn)	Furious Rgan#19m51	21590
1@jorchs,47,249,3	duplicate(dummy_csn)	Furious Rgan#19m52	21590
1@jorchs,31,237,7	duplicate(dummy_csn)	Furious Rgan#19m53	21590
1@jorchs,27,243,5	duplicate(dummy_csn)	Furious Rgan#19m54	21590
1@jorchs,50,243,3	duplicate(dummy_csn)	Furious Rgan#19m55	21590
1@jorchs,39,235,1	duplicate(dummy_csn)	Furious Rgan#19m56	21590

1@jorchs,37,252,5	duplicate(dummy_csn)	Bagot#19m50	4_EP18_BAGOT
1@jorchs,39,271,1	duplicate(dummy_csn)	Leon#19m60	4_EP19_LEON
1@jorchs,40,274,1	duplicate(dummy_csn)	Horuru#19m60	4_M_REPAIR
1@jorchs,38,272,1	duplicate(dummy_csn)	Aurelie#19m60	4_EP19_AURELIE
1@jorchs,36,272,7	duplicate(dummy_csn)	Voglinde#19m60	4_EP19_VOGLINDE
1@jorchs,35,271,7	duplicate(dummy_csn)	Miriam#19m60	4_EP18_MIRIAM

1@jorchs,36,257,0	script	#19m60	WARPNPC,2,2,{
	end;
	
OnTouch:
	.@announce = (!'csn_done && getcharid(1) > 0);
	if (#ConfusedSnakeNest_STATUS == 2 && !checkweight(101183, 1)) {
		dispbottom "[Instancia] Voce esta carregando peso demais para receber as recompensas. Libere espaco e passe pelo portal novamente.";
		end;
	}
	.@finish_result = callfunc("F_InstanceFinishBase", "Ninho da Serpente Confusa", "#ConfusedSnakeNest_STATUS", 30, 6, "", "", 1, 30, 110002, 3000, 3, 0, "Ninho da Serpente Confusa", .@announce);
	if (.@finish_result == -1) {
		dispbottom "[Instancia] Voce esta carregando peso demais para receber as recompensas. Libere espaco e passe pelo portal novamente.";
		end;
	}
	if (.@finish_result == 1) {
		getitem 101183, 1;
		if (!'csn_done)
			'csn_done =1;
	}
	warp "SavePoint",0,0;
end;

OnInstanceInit:
	disablenpc instance_npcname(strnpcinfo(0));
	end;
}

1@jorchs,37,274,5	script	Central Door#19m61	4_ENERGY_BLUE,3,3,{
	if('csn == 20){
		if(!is_party_leader()) end;
		'csn =21;
		enablenpc instance_npcname("Aurelie#19m60");
		enablenpc instance_npcname("Leon#19m60");
		setpcblock PCBLOCK_NPC,true;
		npctalk "Aventureiro, voce esta bem.",instance_npcname("Aurelie#19m60");
		sleep2 1500;
		npctalk "Fiquei preso porque um Rgan superior com muita mana veio de repente na minha direcao.",instance_npcname("Leon#19m60");
		sleep2 1500;
		enablenpc instance_npcname("Horuru#19m60");
		npctalk "Ah~ Isto. Que azar! O que e essa coisa aqui?",instance_npcname("Horuru#19m60");
		sleep2 1500;
		enablenpc instance_npcname("Voglinde#19m60");
		enablenpc instance_npcname("Miriam#19m60");
		npctalk "Parece que todos chegaram.",instance_npcname("Voglinde#19m60");
		sleep2 1500;
		npctalk "Alguem encontrou algo?",instance_npcname("Miriam#19m60");
		sleep2 2000;
		npctalk "Procurei minuciosamente por este lugar complicado, mas nao encontrei nenhum rastro.",instance_npcname("Horuru#19m60");
		sleep2 2000;
		npctalk "Toda a pesquisa organizada das Ilusoes sumiu, e este e o unico lugar que restou.",instance_npcname("Aurelie#19m60");
		sleep2 2000;
		npctalk "O Heart Hunter que capturei me disse que todos foram para o centro.",instance_npcname("Voglinde#19m60");
		sleep2 1500;
		npctalk "Ninguem escapou usando a aeronave. Eles nao vao conseguir.",instance_npcname("Miriam#19m60");
		sleep2 2000;
		npctalk "Este lugar nao e o centro?",instance_npcname("Leon#19m60");
		sleep2 2000;
		npctalk "Entao, se abrirmos esta porta, conseguiremos pegar tanto a Ilusao quanto Lasgand?",instance_npcname("Horuru#19m60");
		sleep2 2000;
		npctalk "Eles abandonaram o lugar onde todos os Rgans de alto grau viviam e foram para um local onde ninguem podia entrar?",instance_npcname("Aurelie#19m60");
		sleep2 1500;
		npctalk "Por que?",instance_npcname("Leon#19m60");
		sleep2 1500;
		npctalk "Vamos abrir. E perguntar diretamente a eles.",instance_npcname("Horuru#19m60");
		sleep2 1500;
		specialeffect EF_SCREEN_QUAKE,AREA,instance_npcname("Central Door#19m61");
		npctalk "Isto nao esta trancado demais?",instance_npcname("Leon#19m60");
		sleep2 2500;
		npctalk "Nao e so este lugar, e? Eles trouxeram tudo que tinham.",instance_npcname("Horuru#19m60");
		sleep2 2500;
		specialeffect EF_M03,AREA,instance_npcname("Central Door#19m61");
		sleep2 500;
		specialeffect EF_NPC_EARTHQUAKE,AREA,instance_npcname("Central Door#19m61");
		sleep2 500;
		specialeffect EF_HYOUSYOURAKU,AREA,instance_npcname("Central Door#19m61");
		specialeffect EF_DESPERADO,AREA,instance_npcname("Central Door#19m61");
		specialeffect EF_SPREADATTACK ,AREA,instance_npcname("Central Door#19m61");
		specialeffect EF_DRAGONFEAR,AREA,instance_npcname("Central Door#19m61");
		sleep2 1000;
		npctalk "O que eles fizeram com esta porta? Abra! Abra!!! Eu sei que eles estao la dentro!!",instance_npcname("Horuru#19m60");
		sleep2 2000;
		npctalk "Nao consigo abri-la nem com a chave que peguei do Heart Hunter.",instance_npcname("Voglinde#19m60");
		sleep2 1500;
		npctalk "E o mesmo quando voce injeta mana.",instance_npcname("Aurelie#19m60");
		sleep2 1500;
		npctalk "Isto nao e uma fortaleza impenetravel?",instance_npcname("Horuru#19m60");
		sleep2 2000;
		npctalk "Entao vou montar vigilancia e ir embora por hoje.",instance_npcname("Voglinde#19m60");
		sleep2 2000;
		npctalk "E o ninho dos Rgans? Devemos queima-lo ate o chao?",instance_npcname("Horuru#19m60");
		sleep2 2000;
		npctalk "Bem, e melhor deixa-lo como esta.",instance_npcname("Aurelie#19m60");
		sleep2 2500;
		npctalk "Para fazer eles acreditarem que desistimos e fomos embora?",instance_npcname("Leon#19m60");
		sleep2 2500;
		npctalk "Sim. Assim talvez eles abram a porta em algum momento e sejam pegos desprevenidos.",instance_npcname("Aurelie#19m60");
		sleep2 2000;
		npctalk "Nao gosto muito dessa ideia, mas nao tem jeito. Teremos que reforcar a vigilancia aqui.",instance_npcname("Horuru#19m60");
		sleep2 2000;
		npctalk "Vamos. Mas antes precisamos lidar com algo se movendo la embaixo.",instance_npcname("Aurelie#19m60");
		if(isbegin_quest(17648) == 1){
			completequest 17648;
			setquest 17649;
		}
		setpcblock PCBLOCK_NPC,false;
		donpcevent instance_npcname("#csn_faceworm_queen") + "::OnStart";
		end;
	}
	end;
}

1@jorchs,1,1,0	script	#csn_faceworm_queen	-1,{
	end;

OnStart:
	killmonster 'map$, instance_npcname(strnpcinfo(0)) + "::OnMyMobDead";
	mapannounce 'map$, "Uma Rainha Faceworm enfurecida surgiu no fundo do ninho!",bc_map,"0xff7777";
	monster 'map$,39,260,"Faceworm Queen",2529,1,instance_npcname(strnpcinfo(0)) + "::OnMyMobDead";
	'csn_final_boss = $@mobid[0];
	setunitdata 'csn_final_boss, UMOB_LEVEL, 180;
	setunitdata 'csn_final_boss, UMOB_MAXHP, 500000000;
	setunitdata 'csn_final_boss, UMOB_HP, 500000000;
	setunitdata 'csn_final_boss, UMOB_ATKMIN, 55000;
	setunitdata 'csn_final_boss, UMOB_ATKMAX, 65000;
	setunitdata 'csn_final_boss, UMOB_MATKMIN, 55000;
	setunitdata 'csn_final_boss, UMOB_MATKMAX, 65000;
	setunitdata 'csn_final_boss, UMOB_DEF, 150;
	setunitdata 'csn_final_boss, UMOB_MDEF, 90;
	setunitdata 'csn_final_boss, UMOB_STR, 5000;
	setunitdata 'csn_final_boss, UMOB_AGI, 120;
	setunitdata 'csn_final_boss, UMOB_VIT, 250;
	setunitdata 'csn_final_boss, UMOB_INT, 5000;
	setunitdata 'csn_final_boss, UMOB_DEX, 5000;
	setunitdata 'csn_final_boss, UMOB_LUK, 150;
	specialeffect EF_DUSTSTORM,AREA,instance_npcname("Central Door#19m61");
	end;

OnMyMobDead:
	if(mobcount('map$,instance_npcname(strnpcinfo(0)) + "::OnMyMobDead")) end;
	mapannounce 'map$, "A Rainha Faceworm foi derrotada. O portal de saida foi aberto.",bc_map,"0xffff66";
	enablenpc instance_npcname("#19m60");
	specialeffect EF_ELECTRIC4,AREA,instance_npcname("#19m60");
	specialeffect EF_YELLOWFLY3,AREA,instance_npcname("#19m60");
	end;

OnInstanceInit:
	'csn_final_boss = 0;
	end;
}


1@jorchs,1,1,0	script	#191_csn_mob	-1,{
	end;
	
OnSummon:
	'clear =0;
	setarray .@name$,"Twisted","Crushed","Deformed","Split";
	switch('summon){
		case 0:
			setarray .@mob,
			221,37,21535,
			237,61,21535,
			242,66,21535,
			204,38,21535,
			181,48,21535,
			135,29,21535,
			124,26,21535;
			break;
			
		case 1:
			setarray .@mob,
			39,76,21535,
			26,90,21535,
			31,59,21535,
			51,45,21535,
			96,64,21535,
			75,23,21535,
			96,64,21535,
			119,80,21535,
			118,103,21535;
			break;
			
		case 2:
			monster 'map$,142,144,"Howling Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,159,151,"Twisted Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,180,139,"Agonizing Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,200,131,"Deformed Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,216,135,"Crushed Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,226,141,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,236,151,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			end;
			
		case 3:
			monster 'map$,228,211,"Twisted Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,245,235,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,210,204,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,194,200,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,173,179,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,247,278,"Combined Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,224,273,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,198,272,"Crushed Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,270,198,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,273,254,"Howling Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			end;
			
		case 4:
			monster 'map$,25,172,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,24,188,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,82,221,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,86,165,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,79,154,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,77,146,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,32,135,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,18,133,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,103,215,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,45,153,"Twisted Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			monster 'map$,121,190,"Crushed Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
			end;
	}
	for(.@i = 0; .@i < getarraysize(.@mob); .@i += 3)
		monster 'map$,.@mob[.@i],.@mob[.@i+1],.@name$[rand(4)] + " Rgan",.@mob[.@i+2],1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
end;

OnMobKill:
	if(mobcount('map$,instance_npcname(strnpcinfo(0)) + "::OnMobKill")) end;
	.@id = 'summon;
	'summon =.@id + 1;
	'clear =1;
	switch(.@id){
		case 0:
			if(!'search)
				instance_announce false,"Alguem... me ajude...",bc_map,0x00FFCC;
			else
				donpcevent instance_npcname("#191_csn_control") + "::" + "OnEvent01";
			end;
			
		case 1:
			if('search){
				'csn =4;
				instance_announce false,"Wah!!! O que e isto!!!",bc_map,0x00FFCC;
				enablenpc instance_npcname("Horuru#19m20");
				enablenpc instance_npcname("Iwin#19m20");
			}
			break;
			
		case 2:
			'csn =7;
			enablenpc instance_npcname("Voglinde#19m30");
			enablenpc instance_npcname("Miriam#19m30");
			break;
			
		case 3:
			if('dev_1 == 0 && 'dev_2 == 0)
				donpcevent instance_npcname("#191_csn_control") + "::" + "OnEvent02";
			else
				instance_announce false,"Ainda ha Rgans por ali!",bc_map,0x00FFCC;
			break;
			
		case 4:
			'csn ='csn + 1;
			if('csn == 18)
				donpcevent instance_npcname("#191_csn_control") + "::" + "OnEvent03";
			break;
	}
end;

OnSummon01:
	monster 'map$,119,123,"Modified Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill01";
	monster 'map$,123,124,"Modified Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill01";
	monster 'map$,120,127,"Crushed Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill01";
	monster 'map$,124,125,"Agonizing Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill01";
end;

OnMobKill01:
	if(mobcount('map$,instance_npcname(strnpcinfo(0)) + "::OnMobKill01")) end;
	hideoffnpc instance_npcname("Heart Hunter#19m20");
	instance_announce false,"?- Acha que vai acabar assim? Nao e como se nao estivessemos preparados!",bc_map,0x00FFCC;
	sleep 1500;
	donpcevent instance_npcname(strnpcinfo(0)) + "::" + "OnSummon02";
end;

OnSummon02:
	'count =5;
	disablenpc instance_npcname("Heart Hunter#19m20");
	hideonnpc instance_npcname("Twisted Rgan#19m20");
	hideonnpc instance_npcname("Howling Rgan#19m22");
	hideonnpc instance_npcname("Agonizing Rgan#19m24");
	hideonnpc instance_npcname("Deformed Rgan#19m25");
	monster 'map$,119,128,"Twisted Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill02";
	monster 'map$,125,124,"Deformed Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill02";
	sleep 3000;
	hideonnpc instance_npcname("Crushed Rgan#19m21");
	hideonnpc instance_npcname("Modified Superior Rgan#19m23");
	hideonnpc instance_npcname("Twisted Rgan#19m26");
	monster 'map$,121,126,"Howling Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill02";
	monster 'map$,124,124,"Twisted Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill02";
	monster 'map$,123,126,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill02";
end;

OnMobKill02:
	'count ='count - 1;
	if('count) end;
	'csn =6;
	hideoffnpc instance_npcname("Horuru#19m20");
	hideoffnpc instance_npcname("Iwin#19m20");
	hideoffnpc instance_npcname("Iwin#19m21");
	npctalk "Ho- Voce cuidou de tudo. De qualquer forma, havia muitos dispositivos desconhecidos por toda parte.",instance_npcname("Horuru#19m20");
	sleep 2500;
	npctalk "Peguei um Heart Hunter tentando opera-lo. O que ele estava tentando fazer?",instance_npcname("Horuru#19m20");
	sleep 2500;
	npctalk "Como o Heart Hunter passou por aquele monte de Rgans?",instance_npcname("Iwin#19m20");
	sleep 2500;
	npctalk "Talvez o Heart Hunter estivesse fazendo algo para evitar ou manipular os Rgans.",instance_npcname("Horuru#19m20");
	sleep 2500;
	npctalk "Entao foi por isso que as paredes de Rgans se desfizeram quando eliminamos os Heart Hunters!",instance_npcname("Iwin#19m21");
	sleep 2500;
	npctalk "Nao vai ser facil. Aventureiro, vamos compartilhar essa informacao com os outros. Vamos.",instance_npcname("Horuru#19m20");
	sleep 500;
	disablenpc instance_npcname("Horuru#19m20");
	disablenpc instance_npcname("Iwin#19m20");
	disablenpc instance_npcname("Iwin#19m21");
	setcell 'map$,120,129,120,129,CELL_WALKABLE,true;
	setcell 'map$,121,128,121,128,CELL_WALKABLE,true;
	setcell 'map$,122,127,122,127,CELL_WALKABLE,true;
	setcell 'map$,123,127,123,127,CELL_WALKABLE,true;
	setcell 'map$,124,126,124,126,CELL_WALKABLE,true;
	setcell 'map$,125,125,125,125,CELL_WALKABLE,true;
	setcell 'map$,126,125,126,125,CELL_WALKABLE,true;
	setcell 'map$,120,129,120,129,CELL_SHOOTABLE,true;
	setcell 'map$,121,128,121,128,CELL_SHOOTABLE,true;
	setcell 'map$,122,127,122,127,CELL_SHOOTABLE,true;
	setcell 'map$,123,127,123,127,CELL_SHOOTABLE,true;
	setcell 'map$,124,126,124,126,CELL_SHOOTABLE,true;
	setcell 'map$,125,125,125,125,CELL_SHOOTABLE,true;
	setcell 'map$,126,125,126,125,CELL_SHOOTABLE,true;
	specialeffect EF_WIND,AREA,instance_npcname("Twisted Rgan#19m20");
	specialeffect EF_WIND,AREA,instance_npcname("Crushed Rgan#19m21");
	specialeffect EF_WIND,AREA,instance_npcname("Howling Rgan#19m22");
	specialeffect EF_WIND,AREA,instance_npcname("Modified Superior Rgan#19m23");
	specialeffect EF_WIND,AREA,instance_npcname("Agonizing Rgan#19m24");
	specialeffect EF_WIND,AREA,instance_npcname("Deformed Rgan#19m25");
	donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon";
	sleep 10000;
	disablenpc instance_npcname("Twisted Rgan#19m20");
	disablenpc instance_npcname("Howling Rgan#19m22");
	disablenpc instance_npcname("Agonizing Rgan#19m24");
	disablenpc instance_npcname("Deformed Rgan#19m25");
	disablenpc instance_npcname("Crushed Rgan#19m21");
	disablenpc instance_npcname("Modified Superior Rgan#19m23");
	disablenpc instance_npcname("Twisted Rgan#19m26");
end;

OnSummon03:
	'count =7;
	disablenpc instance_npcname("Modified Superior Rgan#19m30");
	disablenpc instance_npcname("Twisted Rgan#19m37");
	monster 'map$,249,176,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill03";
	monster 'map$,242,173,"Twisted Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill03";
	sleep 3000;
	disablenpc instance_npcname("Twisted Rgan#19m31");
	disablenpc instance_npcname("Crushed Rgan#19m35");
	monster 'map$,248,175,"Twisted Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill03";
	monster 'map$,244,175,"Crushed Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill03";
	sleep 3000;
	disablenpc instance_npcname("Combined Rgan#19m32");
	disablenpc instance_npcname("Modified Superior Rgan#19m33");
	disablenpc instance_npcname("Modified Superior Rgan#19m34");
	monster 'map$,247,175,"Combined Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill03";
	monster 'map$,246,175,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill03";
	monster 'map$,245,175,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill03";
end;

OnMobKill03:
	'count ='count - 1;
	if('count) end;
	enablenpc instance_npcname("Injection Device#19m30");
end;

OnSummon04:
	monster 'map$,224,151,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill04";
	unittalk $@mobid[0],"O dispositivo esta funcionando, mas por que so ha um animal modificado?";
	monster 'map$,225,141,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill04";
	unittalk $@mobid[0],"Voce desligou o dispositivo? Eles nao disseram que ele murcharia e morreria tao rapido!!!";
	monster 'map$,217,141,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill04";
	unittalk $@mobid[0],"O que? Onde esta todo mundo? Foi voce que me chamou?";
	monster 'map$,230,160,"Unstable Modified Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill04";
end;

OnMobKill04:
	if(mobcount('map$,instance_npcname(strnpcinfo(0)) + "::OnMobKill04")) end;
	'csn =10;
	instance_announce false,"Um dispositivo de injecao adicional foi ativado",bc_map,0x00FFCC;
	enablenpc instance_npcname("Injection Device#19m31");
	enablenpc instance_npcname("Injection Device#19m32");
	'dev_1 =1;
	'dev_2 =1;
	donpcevent instance_npcname("Injection Device#19m31") + "::" + "OnSummon";
	donpcevent instance_npcname("Injection Device#19m32") + "::" + "OnSummon";
	donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon";
	setcell 'map$,249,177,249,177,CELL_WALKABLE,true;
	setcell 'map$,248,176,248,176,CELL_WALKABLE,true;
	setcell 'map$,247,176,247,176,CELL_WALKABLE,true;
	setcell 'map$,246,176,246,176,CELL_WALKABLE,true;
	setcell 'map$,245,176,245,176,CELL_WALKABLE,true;
	setcell 'map$,244,176,244,176,CELL_WALKABLE,true;
	setcell 'map$,243,175,243,175,CELL_WALKABLE,true;
	setcell 'map$,242,174,242,174,CELL_WALKABLE,true;
	setcell 'map$,249,177,249,177,CELL_SHOOTABLE,true;
	setcell 'map$,248,176,248,176,CELL_SHOOTABLE,true;
	setcell 'map$,247,176,247,176,CELL_SHOOTABLE,true;
	setcell 'map$,246,176,246,176,CELL_SHOOTABLE,true;
	setcell 'map$,245,176,245,176,CELL_SHOOTABLE,true;
	setcell 'map$,244,176,244,176,CELL_SHOOTABLE,true;
	setcell 'map$,243,175,243,175,CELL_SHOOTABLE,true;
	setcell 'map$,242,174,242,174,CELL_SHOOTABLE,true;
end;

OnSummon05:
	instance_announce false,"Venha aqui e desligue o dispositivo de injecao.",bc_map,0x00FFCC;
	monster 'map$,191,199,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill05";
	unittalk $@mobid[0],"Eu disse para manter isso funcionando para nao desligar!";
	monster 'map$,199,197,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill05";
	unittalk $@mobid[0],"Nao seria melhor sair daqui?";
	monster 'map$,202,207,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill05";
	unittalk $@mobid[0],"Voce esta em apuros.";
end;

OnMobKill05:
	if(mobcount('map$,instance_npcname(strnpcinfo(0)) + "::OnMobKill05")) end;
	'csn =14;
end;

OnSummon06:
	'count =9;
	disablenpc instance_npcname("Crushed Rgan#19m41");
	disablenpc instance_npcname("Heart Hunter#19m40");
	monster 'map$,141,182,"Crushed Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
	monster 'map$,149,178,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
	monster 'map$,149,179,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
	monster 'map$,150,180,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
	sleep 3000;
	disablenpc instance_npcname("Modified Superior Rgan#19m40");
	disablenpc instance_npcname("Modified Superior Rgan#19m42");
	disablenpc instance_npcname("Modified Superior Rgan#19m43");
	disablenpc instance_npcname("Twisted Rgan#19m44");
	monster 'map$,141,183,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
	monster 'map$,141,181,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
	monster 'map$,141,180,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
	monster 'map$,141,179,"Twisted Rgan",21535,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
	sleep 3000;
	disablenpc instance_npcname("Modified Superior Rgan#19m45");
	monster 'map$,141,178,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill06";
end;

OnMobKill06:
	'count ='count - 1;
	if('count) end;
	setcell 'map$,140,178,140,183,CELL_WALKABLE,true;
	setcell 'map$,140,178,140,183,CELL_SHOOTABLE,true;
	instance_announce false,"A residencia das Ilusoes e perigosa. Alguem...!",bc_map,0x00FFCC;
	donpcevent instance_npcname("#191_csn_mob") + "::" + "OnSummon";
	enablenpc instance_npcname("#e19p03");
	enablenpc instance_npcname("Research Device#19m40");
	enablenpc instance_npcname("Research Device#19m41");
	enablenpc instance_npcname("Laboratory Traces#19m40");
	'dev_4 =1;
	'dev_5 =1;
	donpcevent instance_npcname("Research Device#19m40") + "::" + "OnSummon";
	donpcevent instance_npcname("Research Device#19m41") + "::" + "OnSummon";
end;

OnSummon07:
	monster 'map$,20,136,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill07";
	unittalk $@mobid[0],"Voce fingiu ser um Rgan. Veio aqui para relembrar?";
	monster 'map$,30,139,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill07";
	unittalk $@mobid[0],"Eu disse para nao tocar nisso!";
end;

OnMobKill07:
	if(mobcount('map$,instance_npcname(strnpcinfo(0)) + "::OnMobKill07")) end;
	'csn ='csn + 1;
	if('csn == 18)
		donpcevent instance_npcname("#191_csn_control") + "::" + "OnEvent03";
end;

OnSummon08:
	monster 'map$,26,194,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill08";
	unittalk $@mobid[0],"Voce veio procurar informacoes?";
	monster 'map$,26,188,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill08";
	unittalk $@mobid[0],"Maos para cima!!";
	monster 'map$,19,185,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill08";
	unittalk $@mobid[0],"Nao consegue ver o que vai acontecer?";
end;

OnMobKill08:
	if(mobcount('map$,instance_npcname(strnpcinfo(0)) + "::OnMobKill08")) end;
	'csn ='csn + 1;
	if('csn == 18)
		donpcevent instance_npcname("#191_csn_control") + "::" + "OnEvent03";
end;

OnSummon09:
	'count =26;
	instance_announce false,"A preparacao para injecao de mana esta pronta. O estagio 1 da infusao de mana comecou.",bc_map,0x00FFCC;
	disablenpc instance_npcname("Modified Superior Rgan#19m51");
	disablenpc instance_npcname("Modified Superior Rgan#19m58");
	monster 'map$,35,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,42,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Modified Superior Rgan#19m53");
	disablenpc instance_npcname("Modified Superior Rgan#19m56");
	monster 'map$,37,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,40,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Furious Rgan#19m50");
	disablenpc instance_npcname("Furious Rgan#19m51");
	monster 'map$,31,249,"Furious Rgan",21590,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,47,237,"Furious Rgan",21590,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	removespecialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m50");
	removespecialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m51");
	removespecialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m50");
	removespecialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m51");
	instance_announce false,"A preparacao para injecao de mana esta pronta. O estagio 2 da infusao de mana comecou.",bc_map,0x00FFCC;
	disablenpc instance_npcname("Modified Superior Rgan#19m50");
	disablenpc instance_npcname("Modified Superior Rgan#19m59");
	monster 'map$,34,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,43,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Modified Superior Rgan#19m54");
	disablenpc instance_npcname("Modified Superior Rgan#19m55");
	monster 'map$,38,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,39,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Modified Superior Rgan#19m52");
	disablenpc instance_npcname("Modified Superior Rgan#19m57");
	monster 'map$,36,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,41,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Furious Rgan#19m52");
	disablenpc instance_npcname("Furious Rgan#19m53");
	monster 'map$,47,249,"Furious Rgan",21590,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,31,237,"Furious Rgan",21590,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	removespecialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m52");
	removespecialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m53");
	removespecialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m52");
	removespecialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m53");
	instance_announce false,"A preparacao para injecao de mana esta pronta. O estagio 3 da infusao de mana comecou.",bc_map,0x00FFCC;
	sleep 5000;
	disablenpc instance_npcname("Modified Superior Rgan#19m61");
	disablenpc instance_npcname("Modified Superior Rgan#19m65");
	monster 'map$,35,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,39,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Modified Superior Rgan#19m63");
	disablenpc instance_npcname("Modified Superior Rgan#19m67");
	monster 'map$,37,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,41,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Modified Superior Rgan#19m64");
	disablenpc instance_npcname("Modified Superior Rgan#19m68");
	monster 'map$,38,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,42,254,"Modified Superior Rgan",21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Heart Hunter#19m50");
	disablenpc instance_npcname("Heart Hunter#19m51");
	disablenpc instance_npcname("Heart Hunter#19m52");
	monster 'map$,39,250,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,36,250,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,41,252,"Heart Hunter",21536,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	sleep 5000;
	disablenpc instance_npcname("Furious Rgan#19m55");
	disablenpc instance_npcname("Furious Rgan#19m56");
	monster 'map$,27,243,"Furious Rgan",21590,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,50,243,"Furious Rgan",21590,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	monster 'map$,39,235,"Furious Rgan",21590,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill09";
	removespecialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m54");
	removespecialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m55");
	removespecialeffect EF_NPC_STOP,AREA,instance_npcname("Furious Rgan#19m56");
	removespecialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m54");
	removespecialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m55");
	removespecialeffect EF_GLASSWALL3,AREA,instance_npcname("Furious Rgan#19m56");
end;

OnMobKill09:
	'count ='count - 1;
	if('count) end;
	'csn =20;
	instance_announce false,"Aquela e a residencia de Lasgand?",bc_map,0x00FFCC;
	setcell 'map$,34,255,43,255,CELL_WALKABLE,true;
	setcell 'map$,34,255,43,255,CELL_SHOOTABLE,true;
	enablenpc instance_npcname("Leon#19m60");
	enablenpc instance_npcname("Horuru#19m60");
	enablenpc instance_npcname("Aurelie#19m60");
	enablenpc instance_npcname("Central Door#19m61");
end;
}

1@jorchs,249,79,7	script	Frightened Rgan#19m00	21600,5,5,{
	end;
	
OnTouch:
	if(getd("'" + strnpcinfo(2))) end;
	setd("'" + strnpcinfo(2),1);
	.@id = atoi(replacestr(strnpcinfo(2),"19m0",""));
	switch(.@id){
		case 0:
			hideonnpc instance_npcname("Frightened Rgan#19m00");
			enablenpc instance_npcname("Frightened Rgan#19m50");
			npctalk "Estou com medo... o que ha de errado com meu corpo?",instance_npcname("Frightened Rgan#19m50");
			sleep 2000;
			npctalk "Voce... um humano??? Isso e culpa sua?? O que voce fez comigo?!",instance_npcname("Frightened Rgan#19m50");
			sleep 2000;
			npctalk "Humanos imundos! Devolvam meu corpo...! Agh...!!",instance_npcname("Frightened Rgan#19m50");
			sleep 2000;
			specialeffect EF_FOOD02,AREA,instance_npcname("Frightened Rgan#19m50");
			hideonnpc instance_npcname("Frightened Rgan#19m50");
			break;
			
		case 1:
			hideonnpc instance_npcname("Frightened Rgan#19m01");
			enablenpc instance_npcname("Frightened Rgan#19m51");
			npctalk "Ah, eu deveria estar assim...?",instance_npcname("Frightened Rgan#19m51");
			sleep 2000;
			npctalk "Eu vivi uma boa vida, o que eles fizeram comigo...",instance_npcname("Frightened Rgan#19m51");
			sleep 2000;
			npctalk "Ah... esta frio....",instance_npcname("Frightened Rgan#19m51");
			sleep 2000;
			npctalk "...?! Humano? Humano?? Voce foi quem me transformou nisso???",instance_npcname("Frightened Rgan#19m51");
			sleep 2000;
			specialeffect EF_FOOD02,AREA,instance_npcname("Frightened Rgan#19m51");
			hideonnpc instance_npcname("Frightened Rgan#19m51");
			break;
			
	}
	getmapxy(.@m$,.@x,.@y,BL_NPC);
	monster 'map$,.@x,.@y,strnpcinfo(1),21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
end;

OnMobKill:
	'elite ='elite + 1;
	if('elite == 2){
		'elite =0;
		'search =1;
		if(!'clear)
			instance_announce false,"E melhor fazermos uma busca mais cuidadosa.",bc_map,0x00FFCC;
		else
			donpcevent instance_npcname("#191_csn_control") + "::" + "OnEvent01";
	}
	disablenpc instance_npcname(strnpcinfo(0));
end;
}

1@jorchs,114,20,3	duplicate(Frightened Rgan#19m00)	Frightened Rgan#19m01	21600,5,5
1@jorchs,249,79,7	duplicate(dummy_csn)	Frightened Rgan#19m50	21600
1@jorchs,114,20,3	duplicate(dummy_csn)	Frightened Rgan#19m51	21600

1@jorchs,23,57,5	script	Stranded Rgan#19m10	21600,5,5,{
	end;
	
OnTouch:
	if(getd("'" + strnpcinfo(2))) end;
	setd("'" + strnpcinfo(2),1);
	.@id = atoi(replacestr(strnpcinfo(2),"19m1",""));
	switch(.@id){
		case 0:
			hideonnpc instance_npcname("Stranded Rgan#19m10");
			enablenpc instance_npcname("Stranded Rgan#19m60");
			npctalk "Onde esta todo mundo... Disseram para eu vigiar os ovos, mas nao consigo sair daqui...",instance_npcname("Stranded Rgan#19m60");
			sleep 2000;
			npctalk "Humanos? Humanos nao podem vir aqui! Saiam!",instance_npcname("Stranded Rgan#19m60");
			sleep 2000;
			npctalk "Meu!! Corpo...! Disseram que era um...? Suplemento? ?...!!",instance_npcname("Stranded Rgan#19m60");
			sleep 2000;
			specialeffect EF_FOOD02,AREA,instance_npcname("Frightened Rgan#19m51");
			hideonnpc instance_npcname("Stranded Rgan#19m60");
			break;
			
		case 1:
			hideonnpc instance_npcname("Stranded Rgan#19m11");
			enablenpc instance_npcname("Stranded Rgan#19m61");
			npctalk "Preciso correr para o centro. Como chego la? Tem alguns barulhos estranhos...",instance_npcname("Stranded Rgan#19m61");
			sleep 2000;
			npctalk "Quando vou mudar? Preciso me transformar para passar por isso.",instance_npcname("Stranded Rgan#19m61");
			sleep 2000;
			specialeffect EF_FOOD02,AREA,instance_npcname("Frightened Rgan#19m51");
			npctalk "Oh! Finalmente!!! Terei a chance de me tornar um Bispo...!!! ?!!!",instance_npcname("Stranded Rgan#19m61");
			hideonnpc instance_npcname("Stranded Rgan#19m61");
			break;
	}
	getmapxy(.@m$,.@x,.@y,BL_NPC);
	monster 'map$,.@x,.@y,strnpcinfo(1),21534,1,instance_npcname(strnpcinfo(0)) + "::OnMobKill";
end;

OnMobKill:
	'elite ='elite + 1;
	if('elite == 2){
		'elite =0;
		'search =1;
		if(!'clear)
			instance_announce false,"Ainda ha Rgans na area de busca. E melhor elimina-los rapido.",bc_map,0x00FFCC;
		else {
			'csn =4;
			instance_announce false,"Wah!!! O que e isto!!!",bc_map,0x00FFCC;
			enablenpc instance_npcname("Horuru#19m20");
			enablenpc instance_npcname("Iwin#19m20");
		}
	}
	disablenpc instance_npcname(strnpcinfo(0));
end;
}

1@jorchs,21,97,5	duplicate(Stranded Rgan#19m10)	Stranded Rgan#19m11	21600,5,5
1@jorchs,23,57,5	duplicate(dummy_csn)	Stranded Rgan#19m60	21600
1@jorchs,21,97,5	duplicate(dummy_csn)	Stranded Rgan#19m61	21600
