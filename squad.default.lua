------------------------------------------------------------------------------
-- First names taken from http://www.babynameworld.com/english.asp
-- Last names taken from http://www.last-names.net/origincat.asp?origincat=European
------------------------------------------------------------------------------

local first_names = {
    "Abelard", "Ackley", "Acton", "Addison", "Afton", "Aida", "Aidan", "Ailen", 
    "Aland", "Alcott", "Alden", "Alder", "Aldercy", "Aldis", "Aldrich", "Alfred", 
    "Allard", "Alvin", "Amaris", "Amberjill", "Amherst", "Amsden", "Ansley", 
    "Ashley", "Atherol", "Atwater", "Atwood", "Audrey", "Avena", "Averill", 
    "Ballard", "Bancroft", "Barclay", "Barden", "Barnett", "Baron", "Barse", 
    "Barton", "Baul", "Bavol", "Baxter", "Beacher", "Beaman", "Beardsley", 
    "Beccalynn", "Bede", "Beldon", "Benson", "Bentley", "Benton", "Bersh", 
    "Bethshaya", "Beval", "Beverly", "Birch", "Bishop", "Blade", "Blaine", 
    "Blake", "Blossom", "Blythe", "Bob", "Bolton", "Bond", "Booker", "Booth", 
    "Borden", "Bowman", "Braden", "Bradford", "Bradley", "Bramwell", "Brandon", 
    "Bray", "Brayden", "Brenda", "Brennan", "Brent", "Brett", "Brewster", "Brigham", 
    "Brinley", "Brishen", "Brock", "Broderick", "Bromley", "Bronson", "Brook", 
    "Brown", "Buck", "Buckley", "Bud", "Bunny", "Burdette", "Burgess", "Burle", 
    "Burne", "Burt", "Burton", "Calder", "Caldwell", "Calhoun", "Calvert", 
    "Cam", "Cameron", "Carleton", "Carling", "Carlisle", "Carlton", "Carlyle", 
    "Carrington", "Carter", "Carver", "Chad", "Chal", "Channing", "Chapman", 
    "Charles", "Chatwin", "Chelsea", "Chilton", "Claiborne", "Clark", "Clayton", 
    "Cleveland", "Clifford", "Clinton", "Clive", "Clovis", "Cody", "Colby", 
    "Cole", "Coleman", "Collier", "Colton", "Columbia", "Corin", "Corliss", 
    "Coty", "Courtland", "Courtney", "Creighton", "Crosby", "Culver", "Currier", 
    "Cynric", "Dale", "Dallin", "Dalton", "Damon", "Dane", "Danior", "Daralis", 
    "Darnell", "Darrel", "Darren", "Darthmouth", "Darwin", "Dawn", "Dayton", 
    "Demelza", "Dempster", "Denley", "Denton", "Denver", "Derwin", "Devon", 
    "Dickinson", "Digby", "Dixie", "Donald", "Dooriya", "Dorset", "Dory", "Dover", 
    "Drake", "Duane", "Dudley", "Dugan", "Dunstan", "Durriken", "Durward", 
    "Dustin", "Dwennon", "Dwight", "Eartha", "Easter", "Eaton", "Ebony", "Edda", 
    "Edgardo", "Edison", "Edlyn", "Edmond", "Edolie", "Edsel", "Edward", "Edward", 
    "Egerton", "Elden", "Eldon", "Eldridge", "Ella", "Elmar", "Elton", "Ember", 
    "Emerson", "Emmett", "Ena", "Erika", "Erskine", "Esmeralda", "Esmond", 
    "Ewing", "Fairfax", "Falkner", "Farley", "Farrah", "Farrah", "Farrell", 
    "Fear", "Fenton", "Fern", "Fielding", "Finlay", "Fleming", "Fleta", "Fletcher", 
    "Floyd", "Forbes", "Ford", "Forrester", "Free", "Fuller", "Fulton", "Gage", 
    "Gail", "Gaines", "Garfield", "Garrick", "Garridan", "Gary", "Garyson", 
    "Geoffrey", "Gleda", "Goldie", "Gordon", "Granger", "Grayson", "Gresham", 
    "Grover", "Gypsy", "Gytha", "Hadden", "Hale", "Hall", "Halsey", "Halton", 
    "Hamilton", "Hanley", "Harden", "Harley", "Harman", "Harmony", "Harold", 
    "Harper", "Harrison", "Hartley", "Harva", "Harvey", "Hayden", "Hayes", 
    "Haylee", "Hazel", "Heath", "Heather", "Hilton", "Holbrook", "Holly", "Holt", 
    "Honey", "Hope", "Houston", "Howard", "Hugh", "Hunter", "Huntley", "Ida", 
    "India", "Ives", "Jagger", "Jal", "James", "Jamie", "Jamison", "Jarman", 
    "Jarvis", "Jillian", "Jocelyn", "Jonesy", "Joy", "Kaelyn", "Keane", "Keene", 
    "Kell", "Kelsey", "Kemp", "Kenelm", "Kenley", "Kennard", "Kenneth", "Kenrich", 
    "Kent", "Kenton", "Ker", "Keyon", "Kim", "Kimberley", "King", "Kingsley", 
    "Kinsey", "Kipling", "Kipp", "Kirsten", "Kismet", "Knox", "Kody", "Kyla", 
    "Ladd", "Lainey", "Lander", "Landon", "Lane", "Lang", "Langley", "Lari", 
    "Lark", "Latimer", "Lawson", "Lee", "Leigh", "Leighton", "Leland", "Lensar", 
    "Leslie", "Lew", "Liberty", "Lincoln", "Lind", "Lindsay", "Linwood", "Litton", 
    "Llewellyn", "Locke", "London", "Love", "Lowell", "Luella", "Lyman", "Lyndon", 
    "Lyre", "Mac", "Macon", "Macy", "Maida", "Maitane", "Maitland", "Makepeace", 
    "Mala", "Mander", "Manhattan", "Manley", "Manning", "Marden", "Marland", 
    "Marlow", "Marsden", "Marshal", "Mather", "Mavis", "Maxwell", "Mead", "Melor", 
    "Melville", "Mendel", "Mercer", "Mercy", "Merrick", "Merry", "Milburn", 
    "Millard", "Miller", "Milton", "Missy", "Misty", "Morley", "Morven", "Mull", 
    "Nara", "Nash", "Neda", "Nelson", "Nevin", "Newell", "Newman", "Norman", 
    "North", "Nyle", "Oakes", "Oakley", "Ogden", "Olin", "Orman", "Orson", 
    "Osbert", "Osborn", "Osmond", "Oswald", "Oswin", "Oxford", "Packard", "Palma", 
    "Palmer", "Paris", "Parker", "Parr", "Parry", "Paxton", "Payton", "Pearl", 
    "Pebbles", "Pell", "Penley", "Penn", "Pepper", "Perri", "Perry", "Pierce", 
    "Pierson", "Piper", "Poppy", "Prentice", "Prescott", "Preston", "Putnam", 
    "Queen", "Queena", "Queenie", "Quella", "Quenna", "Radcliff", "Radcliffe", 
    "Radella", "Radford", "Rae", "Raleigh", "Ralph", "Ramsey", "Ransford", 
    "Ransley", "Ransom", "Raven", "Ravinger", "Rawlins", "Rayburn", "Raymond", 
    "Read", "Redford", "Reed", "Reeve", "Reeves", "Reginald", "Remington", 
    "Rhett", "Rhodes", "Richard", "Richelle", "Rider", "Ridgley", "Ridley", 
    "Rigby", "Ripley", "Rishley", "Robert", "Roberta", "Rochester", "Rodman", 
    "Rodney", "Roldan", "Rowan", "Rowena", "Royce", "Rudd", "Rudyard", "Ruford", 
    "Rumer", "Russel", "Rutherford", "Ryesen", "Rylan", "Sabrina", "Salal", 
    "Sanborn", "Sanders", "Sandon", "Sanford", "Sawyer", "Scarlet", "Scarlett", 
    "Scott", "Seabert", "Seaton", "Selby", "Severin", "Seward", "Seymour", 
    "Shandy", "Sharman", "Shaw", "Shelby", "Sheldon", "Shelley", "Shepherd", 
    "Sherlock", "Sherman", "Sherwood", "Shipley", "Shirley", "Siddel", "Skeet", 
    "Skye", "Skyla", "Skylar", "Slade", "Smith", "Snowden", "Spalding", "Sparrow", 
    "Spencer", "Spike", "Spring", "Standish", "Stanford", "Stanislaw", "Stanley", 
    "Stanley", "Stanway", "Sterling", "Sterne", "Stockard", "Stoke", "Stokley", 
    "Storm", "Stroud", "Studs", "Summer", "Sunny", "Sutton", "Swain", "Tab", 
    "Tanner", "Tate", "Tatum", "Tawnie", "Taylor", "Telford", "Tem", "Tennyson", 
    "Terrel", "Thane", "Thatcher", "Thistle", "Thorne", "Thorpe", "Thurlow", 
    "Tilden", "Tina", "Todd", "Tomkin", "Townsend", "Tranter", "Tremayne", 
    "Trey", "Tripp", "Trudy", "Truman", "Tucker", "Tuesday", "Turner", "Twain", 
    "Tye", "Tyler", "Tyne", "Udolf", "Ulla", "Ulrich", "Ulrika", "Unity", "Unwin", 
    "Upshaw", "Upton", "Vala", "Vance", "Velvet", "Verity", "Vian", "Wade", 
    "Wakefield", "Walker", "Wallace", "Walton", "Ward", "Warren", "Washington", 
    "Watson", "Waverly", "Wayland", "Waylen", "Wayne", "Webster", "Welcome", 
    "Wells", "Wendy", "Wesley", "West", "Weston", "Wetherby", "Wheaton", "Wheeler", 
    "Whit", "Whitfield", "Whitlaw", "Whitney", "Wilfred", "Willow", "Wilmer", 
    "Wilona", "Winifred", "Winslow", "Winston", "Winter", "Winthrop", "Wolf", 
    "Woodley", "Woodrow", "Woodward", "Wright", "Wyatt", "Wylie", "Wyndam", 
    "Wyndham", "Yardley", "Yates", "Yedda", "Yeoman", "York", "Yule", "Zane", 
    "Zelene", "Zinnia", 
}
local last_names = {
    "Alfort", "Allendorf", "Alsop", "Amaker", "Angus", "Annan", "Annesley", 
    "Appleby", "Arbuthnot", "Armitage", "Artois", "Arundel", "Ashburton", "Astor", 
    "Athill", "Athow", "Auchinleck", "Averill", "Ayres", "Balcombe", "Balfour", 
    "Ballantine", "Ballantyne", "Bancroft", "Bannatyne", "Bar", "Barnum", "Barnwell", 
    "Barringer", "Barrow", "Barry", "Barstow", "Barwick", "Beal", "Beckett", 
    "Beckley", "Beckwith", "Bedford", "Beers", "Bellamy", "Belvidere", "Bentley", 
    "Bethune", "Beveridge", "Beverly", "Biggar", "Billings", "Bingham", "Binney", 
    "Birch", "Birnie", "Blackwood", "Blaisdale", "Blaney", "Bloss", "Bogue", 
    "Bolingbroke", "Bolton", "Bonar", "Boswell", "Boughton", "Bourne", "Boynton", 
    "Bradford", "Braine", "Brandon", "Breck", "Breckenridge", "Brentwood", 
    "Briare", "Bridge", "Brienne", "Brierly", "Brighton", "Brocklesby", "Brodt", 
    "Bromfield", "Bromley", "Brooks", "Brougham", "Broughton", "Bryne", "Buchan", 
    "Buchanan", "Buckbee", "Buddington", "Bulkeley", "Bunnell", "Burbeck", 
    "Burgos", "Burgoyne", "Burleigh", "Burnham", "Burns", "Burrard", "Burton", 
    "Bushwell", "Butman", "Buxton", "Cadwell", "Calder", "Caldwell", "Carden", 
    "Carey", "Carmichael", "Caw", "Caxton", "Cayly", "Chadwick", "Challoner", 
    "Chatham", "Chatsey", "Chatsworth", "Chatterton", "Chedsey", "Chesebrough", 
    "Chester", "Chichester", "Chilton", "Church", "Clauson", "Clavering", "Clay", 
    "Cleveland", "Cliff", "Clifton", "Clum", "Cobb", "Cobern", "Cochran", "Cockburn", 
    "Coffin", "Coggeshall", "Colby", "Collamore", "Colley", "Colquite", "Colton", 
    "Colven", "Conry", "Contin", "Conyers", "Coote", "Corbin", "Cornish", "Cornwallis", 
    "Corrie", "Cotesworth", "Courtenay", "Coventry", "Cranston", "Crawford", 
    "Crayford", "Croft", "Cross", "Crosswell", "Cullen", "Cummings", "Cunningham", 
    "Cupar", "D'Oily", "Daggett", "Dalrymple", "Dalton", "Danforth", "Darby", 
    "Davenport", "De Wilde", "Dearden", "Denio", "Derby", "Devenish", "Devlin", 
    "Digby", "Dinsmor", "Dolbeer", "Dole", "Dorset", "Dudley", "Dumfries", 
    "Dun", "Dunbar", "Dunham", "Dunipace", "Duppa", "Durban", "Durden", "Durham", 
    "Dutton", "Dyke", "Eastcote", "Eberlee", "Edgecumbe", "Elphinstone", "Ely", 
    "Emmet", "Esham", "Eton", "Eure", "Evelyn", "Ewell", "Fairholm", "Fales", 
    "Falun", "Fanshaw", "Farnham", "Fay", "Felton", "Ferrer", "Fife", "Fifield", 
    "Filey", "Flanders", "Fleming", "Flint", "Foote", "Forbes", "Fordham", 
    "Fosdyke", "Fotherby", "Fothergill", "Fotheringham", "Fremont", "Frothingham", 
    "Fulham", "Fulsom", "Gano", "Garfield", "Garnet", "Garrison", "Gaston", 
    "Gavet", "Geddes", "Gihon", "Gill", "Girdwood", "Girvan", "Glanville", 
    "Glentworth", "Gliston", "Gloucester", "Goadby", "Goring", "Goudy", "Grasse", 
    "Gray", "Greely", "Grimsby", "Hadley", "Haineau", "Halden", "Hales", "Halsey", 
    "Halstead", "Ham", "Hamlin", "Hampton", "Hanley", "Hanna", "Harcourt", 
    "Harding", "Hargill", "Harley", "Harrington", "Hartwell", "Hasbrouck", 
    "Hastings", "Hatch", "Hatfield", "Hathaway", "Hatton", "Haverill", "Hayden", 
    "Hedges", "Hedon", "Helling", "Henley", "Herndon", "Hernshaw", "Heyden", 
    "Hinckley", "Hindon", "Hippisley", "Hipwood", "Holbech", "Holland", "Holme", 
    "Holsapple", "Holt", "Holywell", "Hope", "Horton", "Hosford", "Hough", 
    "Houghton", "Houston", "Hubbell", "Huddleston", "Hull", "Hungerford", "Huntley", 
    "Hutton", "Hyde", "Incledon", "Ingham", "Ingleby", "Ipres", "Ireton", "Irving", 
    "Isabel", "Isham", "Islip", "Ives", "Kay", "Keith", "Kelsey", "Kelso", 
    "Kendall", "Kent", "Kettle", "Kilburne", "Kilham", "Kinghorn", "Kingston", 
    "Kinloch", "Kirby", "Kirkaldy", "Kirkham", "Kirkpatrick", "Knox", "Kyle", 
    "Lacy", "Lancaster", "Langton", "Laycock", "Lechmere", "Leigh", "Leland", 
    "Lester", "Leven", "Lewes", "Lewknor", "Lincoln", "Lindall", "Lindsay", 
    "Linn", "Linton", "Lippencot", "Lismore", "Livingstone", "Lonsdale", "Ludlow", 
    "Main", "Mansfield", "Mansle", "Mar", "Massey", "Mayo", "Mead", "Medcaf", 
    "Menteth", "Merton", "Meyeul", "Middleditch", "Middleton", "Milbourne", 
    "Mills", "Milthorpe", "Milton", "Moffatt", "Monroe", "Morley", "Moseley", 
    "Moxley", "Mundy", "Munsel", "Murray", "Nairne", "Nance", "Needham", "Newton", 
    "Nisbett", "Nogent", "Norcutt", "Norfolk", "Northop", "Northumberland", 
    "Norton", "Norwich", "Nottingham", "Oakham", "Ockham", "Ogilvie", "Ollendorff", 
    "Olmstead", "Onslow", "Orchard", "Orr", "Orton", "Otter", "Ouseley", "Oxford", 
    "Pangbourn", "Paris", "Parret", "Parsall", "Parshall", "Paxton", "Pedin", 
    "Peebles", "Peele", "Pelham", "Pendleton", "Peney", "Pennington", "Percy", 
    "Pevensey", "Pickering", "Pickersgill", "Playfair", "Pleasants", "Polley", 
    "Pollock", "Poole", "Poulton", "Pressley", "Preston", "Pringle", "Radcliff", 
    "Ralston", "Ramsey", "Ranney", "Rawdon", "Reddenhurst", "Rhodes", "Riddell", 
    "Ripley", "Rochester", "Romanno", "Root", "Rowe", "Rowen", "Rue", "Rusbridge", 
    "Rutherford", "Rynders", "Safford", "Sandford", "Saterlee", "Scarret", 
    "Scroggs", "Seaford", "Seaforth", "Seaton", "Selby", "Selkirk", "Seton", 
    "Severn", "Shaddock", "Shelley", "Sheppy", "Shiel", "Shrewsbury", "Shuckburgh", 
    "Shurtliff", "Shute", "Slade", "Snodgrass", "Soule", "Southwell", "Spalding", 
    "St. Albans", "Stanhope", "Stanley", "Stebbins", "Stein", "Stirling", "Stocking", 
    "Stockton", "Stokes", "Stokesby", "Stone", "Stoughton", "Strain", "Sunderland", 
    "Surtees", "Sutton", "Swift", "Swinburn", "Symington", "Tabor", "Tattersall", 
    "Teddington", "Teesdale", "Tefft", "Telfair", "Telford", "Temes", "Thorn", 
    "Thurston", "Thwaite", "Tichenor", "Tillinghast", "Tilton", "Ting", "Torry", 
    "Toucey", "Tournay", "Tracy", "Troublefield", "Trowbridge", "True", "Tuthill", 
    "Twickenham", "Van Alstyne", "Van Amee", "Van Arden", "Van Arnhem", "Van Dam", 
    "Van Horn", "Van Huisen", "Van Ingen", "Van Loon", "Van Ness", "Van Rensselaer", 
    "Van Schaack", "Van Slyck", "Van Stantvoordt", "Van Volkenburg", "Van Vorst", 
    "Van Woert", "Van Wyck", "Vesey", "Vielle", "Vine", "Waite", "Wakefield", 
    "Wallop", "Walpole", "Walton", "Wample", "Wands", "Warburton", "Wardlaw", 
    "Ware", "Warwick", "Washington", "Wassen", "Waters", "Way", "Wayland", 
    "Weeden", "Welby", "Welden", "Wells", "Wemyss", "Wentworth", "Westmoreland", 
    "Wetherby", "Wetherspoon", "Wheaton", "Whittaker", "Wigan", "Wilberforce", 
    "Willoughby", "Wilton", "Wiltshire", "Winch", "Winchester", "Windham", 
    "Windsor", "Wing", "Wingfield", "Winslow", "Winterton", "Winthrop", "Wire", 
    "Wiswall", "Wolsey", "Woolley", "Worcester", "York", 
}

local sectoid_names = {
    "Sectoid Soldier", "Sectoid Navigator", "Sectoid Engineer", "Sectoid Medic", 
    "Sectoid Leader", "Sectoid Commander",
}

local muton_names = {
    "Muton Soldier", "Muton Navigator", "Muton Engineer", "Muton Medic", 
    "Muton Leader", "Muton Commander",
}

local floater_names = {
    "Floater Soldier", "Floater Navigator", "Floater Engineer", "Floater Medic", 
    "Floater Leader", "Floater Commander",
}

local snakeman_names = {
    "Snakeman Soldier", "Snakeman Navigator", "Snakeman Engineer", "Snakeman Medic", 
    "Snakeman Leader", "Snakeman Commander",
}

local ethereal_names = {
    "Ethereal Soldier", "Ethereal Navigator", "Ethereal Engineer", "Ethereal Medic", 
    "Ethereal Leader", "Ethereal Commander",
}

local function get_random_name()
    return random(first_names) .. " " .. random(last_names)
end
local function get_random_sectoid_name()
    return random(sectoid_names)
end
local function get_random_muton_name()
    return random(muton_names)
end
local function get_random_floater_name()
    return random(floater_names)
end
local function get_random_snakeman_name()
    return random(snakeman_names)
end
local function get_random_ethereal_name()
    return random(ethereal_names)
end

return {
    {
        Name = get_random_name(),
        SkinType = 10,
        fFemale = 0,
        Appearance = 2,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
                {0, 0, "SMOKE VIAL"},
                {1, 0, "FRAGMENTATION MINE"},
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {1, 0, "PLASMA HANDGUN", "PLASMA HANDGUN CELL"},
            },
            ["LEFT HAND"] = {
                {1, 1, "ALIEN FUSION GRENADE"},
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
                {0, 0, "PLASMA HANDGUN CELL"},
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_ethereal_name(),
        SkinType = 8,
        fFemale = 0,
        Appearance = 0,
        Attributes = {
            TimeUnits = 69,
            Health = 61,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {1, 0, "PLASMA HANDGUN", "PLASMA HANDGUN CELL"},
            },
            ["LEFT HAND"] = {
                {0, 0, "MISSLE LAUNCHER", "HEAVY MISSLE"},
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 10,
        fFemale = 0,
        Appearance = 3,
        Attributes = {
            TimeUnits = 69,
            Health = 51,
            Stamina = 70,
            Reactions = 50,
            Strength = 25,
            Firing = 70,
            Throwing = 60,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {1, 0, "PLASMA HANDGUN", "PLASMA HANDGUN CELL"},
            },
            ["LEFT HAND"] = {
                {0, 1, "ALIEN FUSION GRENADE"},
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
                {0, 0, "PLASMA HANDGUN CELL"},
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_snakeman_name(),
        SkinType = 7,
        fFemale = 0,
        Appearance = 0,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {0, 1, "PLASMA HANDGUN", "PLASMA HANDGUN CELL"},
            },
            ["LEFT HAND"] = {
                {0, 1, "MAGNETIC FUSION LAUNCHER", "MAGNETIC FUSION MISSLE"},
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 3,
        fFemale = 0,
        Appearance = 2,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {0, 0, "XSC HEAVY LASER RIFLE"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
                {0, 0, "XSC LASER PISTOL"},
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 3,
        fFemale = 0,
        Appearance = 0,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {0, 0, "PLASMA HANDGUN", "PLASMA HANDGUN CELL"},
            },
            ["LEFT HAND"] = {
                {0, 0, "XAAS 12 GUAGE", "XAAS HE-AMMO"},
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_floater_name(),
        SkinType = 9,
        fFemale = 0,
        Appearance = 0,
        Attributes = {
            TimeUnits = 69,
            Health = 51,
            Stamina = 70,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
                {0, 0, "ALIEN FUSION GRENADE"},
                {1, 0, "ALIEN FUSION GRENADE"},
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {0, 0, "PLASMA BLASTER", "PLASMA BLASTER CELL"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
                {0, 0, "PLASMA BLADE"},
                {1, 0, "PLASMA BLASTER CELL"},
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 2,
        fFemale = 1,
        Appearance = 1,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {0, 0, "XSC LASER RIFLE"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 2,
        fFemale = 0,
        Appearance = 1,
        Attributes = {
            TimeUnits = 69,
            Health = 50,
            Stamina = 59,
            Reactions = 62,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
                {0, 0, "DEMOLITION PACK"},
            },
            ["LEFT SHOULDER"] = {
                {1, 0, "FRAGMENTATION MINE"},
                {0, 0, "SMOKE VIAL"},
            },
            ["RIGHT HAND"] = {
                {0, 0, "XSC HEAVY LASER RIFLE"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 2,
        fFemale = 0,
        Appearance = 0,
        Attributes = {
            TimeUnits = 69,
            Health = 80,
            Stamina = 50,
            Reactions = 50,
            Strength = 25,
            Firing = 71,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
                {0, 0, "SMOKE VIAL"},
                {1, 0, "SMOKE VIAL"},
            },
            ["RIGHT HAND"] = {
                {0, 0, "SILVER TALON .45 MAGNUM", ".45 MAGNUM AMMO"},
            },
            ["LEFT HAND"] = {
                {0, 1, "DEMOLITION PACK"},
            },
            ["RIGHT LEG"] = {
                {0, 0, ".45 MAGNUM AMMO"},
                {1, 0, ".45 MAGNUM AMMO"},
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
                {0, 0, "COMBAT KNIFE"},
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 2,
        fFemale = 1,
        Appearance = 0,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {1, 0, "XSC LASER RIFLE"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 1,
        fFemale = 0,
        Appearance = 3,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {1, 0, "XSC LASER RIFLE"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_name(),
        SkinType = 2,
        fFemale = 1,
        Appearance = 2,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
                {1, 0, "ALIEN FUSION GRENADE"},
                {0, 0, "SMOKE BOMB"},
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {1, 0, "XSC LASER RIFLE"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_sectoid_name(),
        SkinType = 5,
        fFemale = 0,
        Appearance = 0,
        Attributes = {
            TimeUnits = 79,
            Health = 51,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
            },
            ["LEFT SHOULDER"] = {
            },
            ["RIGHT HAND"] = {
                {1, 0, "PLASMA GUN", "PLASMA GUN CELL"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
    {
        Name = get_random_muton_name(),
        SkinType = 6,
        fFemale = 0,
        Appearance = 0,
        Attributes = {
            TimeUnits = 69,
            Health = 61,
            Stamina = 60,
            Reactions = 50,
            Strength = 25,
            Firing = 80,
            Throwing = 50,
        },
        Inventory = {
            ["RIGHT SHOULDER"] = {
                {0, 0, "SMOKE BOMB"},
                {1, 0, "SMOKE BOMB"},
            },
            ["LEFT SHOULDER"] = {
                {1, 0, "SMOKE VIAL"},
                {0, 0, "SMOKE VIAL"},
            },
            ["RIGHT HAND"] = {
                {0, 0, "XSC LASER RIFLE"},
            },
            ["LEFT HAND"] = {
            },
            ["RIGHT LEG"] = {
            },
            ["LEFT LEG"] = {
            },
            ["BACK PACK"] = {
            },
            ["BELT"] = {
            },
            ["GROUND"] = {
            },
        },
    },
}
