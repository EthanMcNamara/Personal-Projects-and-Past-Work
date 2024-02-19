const request = require("supertest");
const baseURL = "http://localhost:3000";

// Tests all GET functions
describe("POST /poll", () => {
    it("should return 405", async () => {
        const response = await request(baseURL).post("/poll");
        expect(response.statusCode).toBe(405);
    });
});

describe("POST /poll/:id", () => {
    const validPoll = "plw2024";
    const invalidPoll = "gibberish";
    it("should return 405 on existing pollId", async () => {
        const response = await request(baseURL).post("/poll/"+validPoll);
        expect(response.statusCode).toBe(405);
    });
    it("should return 405 on invalid pollId", async () => {
        const response = await request(baseURL).post("/poll/"+invalidPoll);
        expect(response.statusCode).toBe(405);
    });
});

describe("POST /vote", () => {
    const validVote = {
        "pollId": "plw2024",
        "optionId": 1
    };
    const invalidVote1 = {
        "pollId": "fakePoll",
        "optionId": 1
    };
    const invalidVote2 = {
        "pollId": "fakePoll",
        "optionId": 4
    };
    const invalidVote3 = {
        "pollId": "fakePoll",
        "optionId": 0
    };
    const invalidVote4 = {
        "pollId": "fakePoll",
        "optionId": -1
    };
    const invalidVote5 = {
        "pollId": "fakePoll",
        "optionId": 10
    };
    var validVoteId = "";
    beforeAll(async () => {
        // Ensure there is at least 1 vote before checking for votes
        const response = await request(baseURL).get("/vote/plw2024");
        voteCount = response.body.length;
    });
    afterAll(async () => {
        // Delete vote to prevent bloating
        const response = await request(baseURL).delete("/vote/"+validVoteId);
    });
    it("should return 200 with valid vote", async () => {
        const response = await request(baseURL).post("/vote").send(validVote);
        validVoteId = response.body;
        expect(response.status).toBe(200);
    });
    it("should return 400 with invalid vote - pollId does not exist", async () => {
        const response = await request(baseURL).post("/vote").send(invalidVote1);
        expect(response.status).toBe(400);
    });
    it("should return 400 with invalid vote - pollId does exist, optionId >= 1 and <=5 but out of poll's range", async () => {
        const response = await request(baseURL).post("/vote").send(invalidVote2);
        expect(response.status).toBe(400);
    });
    it("should return 400 with invalid vote - pollId does exist, optionId == 0 ", async () => {
        const response = await request(baseURL).post("/vote").send(invalidVote3);
        expect(response.status).toBe(400);
    });
    it("should return 400 with invalid vote - pollId does exist, optionId < 0 ", async () => {
        const response = await request(baseURL).post("/vote").send(invalidVote4);
        expect(response.status).toBe(400);
    });
    it("should return 400 with invalid vote - pollId does exist, optionId > 5 ", async () => {
        const response = await request(baseURL).post("/vote").send(invalidVote5);
        expect(response.status).toBe(400);
    });
    it("should return 400 with invalid vote - no body given", async () => {
        const response = await request(baseURL).post("/vote");
        expect(response.status).toBe(400);
    });
});

describe("POST /vote/:id", () => {
    const validPoll = "plw2024";
    const invalidPoll = "gibberish";
    const validVote = {
        "pollId": "plw2024",
        "optionId": 1
    };
    var voteId = "";
    beforeAll(async () => {
        // Ensure there is at least 1 vote before checking for votes
        const response = await request(baseURL).post("/vote").send(validVote);
        voteId = response.body;
    });
    afterAll(async () => {
        // Delete vote to prevent bloating
        const response = await request(baseURL).delete("/vote/"+voteId);
    });
    it("should return 405 even when parameter matches an existing pollId", async () => {
        const response = await request(baseURL).post("/vote/"+validPoll);
        expect(response.status).toBe(405);
    });
    it("should return 405 when parameter does not match an existing pollId", async () => {
        const response = await request(baseURL).post("/vote/"+invalidPoll);
        expect(response.status).toBe(405);
    });
});


describe("POST /*", () => {
    const gibberish = "thisMakesNoSense";
    const validPoll = "plw2024";
    it("should return 404 for empty space after baseURL", async () => {
        const response = await request(baseURL).post("/");
        expect(response.status).toBe(404);
    });
    it("should return 404 if unexpected route given after baseURL", async () => {
        const response = await request(baseURL).post("/"+gibberish);
        expect(response.status).toBe(404);
    });
    it("should return 404 if valid pollId given after baseURL without '/poll/'", async () => {
        const response = await request(baseURL).post("/"+validPoll);
        expect(response.status).toBe(404);
    });
});